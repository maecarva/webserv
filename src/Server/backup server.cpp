void Server::handler()
{

	struct epoll_event events[MAX_EVENTS];
	int nready = epoll_wait(this->_epoll_fd, events, MAX_EVENTS, 0); // return number of events to handle, 0 to prevent blocking for connexion
	if (nready < 0)
	{
		std::perror("epoll_wait failed");
		return;
	}

	for (int i = 0; i < nready; i++) // loop for each events
	{
		int fd = events[i].data.fd;
		/// -------
		if (fd == this->_socketfd) // first case : new connection
		{
			struct sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			int client_fd = accept(this->_socketfd, (struct sockaddr *)&client_addr, &client_len);
			if (client_fd < 0)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				else
				{
					std::perror("accept client failed");
					break;
				}
			}

			if (setNonBlocking(client_fd) < 0)
			{
				std::perror("setNonblicking client_fd");
				close(client_fd);
				continue;
			}

			struct epoll_event client_ev;
			std::memset(&client_ev, 0, sizeof(client_ev));
			client_ev.events = EPOLLIN | EPOLLET;
			client_ev.data.fd = client_fd;
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) < 0)
			{
				std::perror("add client fd to epoll failed");
				close(client_fd);
				continue;
			}

			this->_clientBuffers.insert(std::make_pair(client_fd, Client(client_fd, this->getConfig().getClientMaxBodySize())));
		}
		/// -------
		else // second case : client socket ready for read
		{
			if (events[i].events & EPOLLIN)
			{
				bool closed = false;
				while (true)
				{
					char buf[BUFSIZ];
					ssize_t count = recv(fd, buf, sizeof(buf) - 1, 0); // -1 pour laisser place au \0

					if (count > 0)
					{
						buf[count] = '\0';
						// std::cout << buf << std::endl;
						std::string buffer(buf, count); // Utiliser le constructeur avec taille
						
						// Recherche du Content-Length si pas encore trouvé
						if (this->_clientBuffers[fd].getContentLength() == 0)
						{
							size_t pos = buffer.find("Content-Length:"); // Attention à la casse
							if (pos == std::string::npos)
								pos = buffer.find("content-length:");
							
							if (pos != std::string::npos)
							{
								// Trouver le début du nombre
								size_t start = buffer.find(':', pos) + 1;
								while (start < buffer.size() && isspace(buffer[start])) start++; // Skip whitespace
								
								long len = strtol(buffer.c_str() + start, NULL, 10);
								this->_clientBuffers[fd].setContentLength(len); // Correction du nom de méthode
							}
						}
						
						// Ajouter les données reçues
						this->_clientBuffers[fd].addBodyCount(buffer.c_str(), buffer.size());
						
						// Vérifier si on a tout lu
						if (this->_clientBuffers[fd].getAllRead())
						{
							break;
						}
						continue ;
					}
					else if (count == 0) {
						closed = true;
						break;
					} else {
						if (errno == EAGAIN || errno == EWOULDBLOCK) {
							break;
						}
						// véritable erreur réseau
						std::perror("recv failed");
						closed = true;
						break;
					}
				}

				if (closed)
				{
					// Déconnexion du client : retirer de epoll et fermer
					if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
						std::perror("epoll_ctl DEL");
					close(fd);
					this->_clientBuffers.erase(fd);
					std::ostringstream oss;
					oss << "Client FD=" << fd << " déconnecté";
					Logger::debug(oss.str().c_str());
				}
				else
				{
					if (this->_clientBuffers[fd].getAllRead()) {
						Request req = Request(*this, fd);

						// Respond to client
						std::string reqstr = this->_clientBuffers[fd].getThatBody();

						req.parseRequest(reqstr, *this);
						std::string	reply = req.CreateResponse();
						ssize_t bytes_send = send(fd, reply.c_str(), reply.size(), 0);
						if (bytes_send < 0 || ((size_t)bytes_send != reply.size()))
						{
							if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
								std::perror("epoll_ctl DEL après send");
							close(fd);
							this->_clientBuffers.erase(fd);
							Logger::error("Failed to send reponse.");
							continue;
						}

						// Close connection
						if (req.isKeepAlive() == false)
						{
							if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
								std::perror("epoll_ctl DEL après send");
							close(fd);
							this->_clientBuffers.erase(fd);
						}
						else {
							this->_clientBuffers.erase(fd);
							this->_clientBuffers[fd] = Client(fd, this->getConfig().getClientMaxBodySize());
						}
						req.logRequest(*this);
					}
				}
			}
		}
	}
}