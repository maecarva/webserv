#include "Webserv.hpp"

Server::Server(Config &config) : _config(config)
{
	// * erase last / of rootdir
	std::vector<Route> routes = this->getConfig().getRoutes();
	for (std::vector<Route>::iterator i = routes.begin(); i != routes.end(); i++)
	{
		std::string str = (*i).getRootDir();
		std::string::iterator eit = str.end() - 1;
		if (*eit == '/')
		{
			str.erase(eit);
			(*i).setRootDir(str);
		}
	}

	// * create socket
	int listenfd = -1;
	struct sockaddr_in addr;
	int opt = 1;
	int epoll_fd = -1;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::perror("Socket creation failed");
		throw Server::ServerCreationError();
	}

	// * set non blocking
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::perror("setsockopt failed");
		close(listenfd);
		throw Server::ServerCreationError();
	}

	if (setNonBlocking(listenfd) < 0)
	{
		std::perror("setNonBlocking failed");
		throw Server::ServerCreationError();
	}

	// * set address of socket
	std::string hostaddr = config.getListen();

	size_t pos = hostaddr.find(':');
	if (pos == std::string::npos)
		throw Server::ServerCreationError();

	std::string host = hostaddr.substr(0, pos);
	std::string port = hostaddr.substr(pos + 1, hostaddr.size());
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; // IPv4
	addr.sin_addr.s_addr = inet_addr(host.c_str());
	addr.sin_port = htons(std::atoi(port.c_str()));

	if (bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{ // link listenfd to addr
		std::perror("bind failed");
		close(listenfd);
		throw Server::ServerCreationError();
	}

	if (listen(listenfd, 10) < 0)
	{ // start listening
		std::perror("listen failed");
		close(listenfd);
		throw Server::ServerCreationError();
	}

	std::ostringstream oss;
	oss << "Server is listening on " << hostaddr;
	Logger::debug(oss.str().c_str());

	epoll_fd = epoll_create(1); // create epoll instance
	struct epoll_event ev;
	std::memset(&ev, 0, sizeof(ev));

	ev.events = EPOLLIN;
	ev.data.fd = listenfd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &ev) < 0)
	{
		std::perror("epoll_ctl failed");
		close(listenfd);
		close(epoll_fd);
		throw Server::ServerCreationError();
		return;
	}

	std::memset(&this->_ev, 0, sizeof(this->_ev));

	this->_epoll_fd = epoll_fd;
	this->_socketfd = listenfd;
}

Server::~Server() {
};

Server &Server::operator=(const Server &server)
{
	if (this != &server)
	{
		this->_config = server._config;
		this->_socketfd = server._socketfd;
		this->_epoll_fd = server._epoll_fd;
		this->_ev = server._ev;
		this->_clientBuffers = server._clientBuffers;
	}
	return *this;
}

Config &Server::getConfig()
{
	return this->_config;
}

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

			this->_clientBuffers.insert(std::make_pair(client_fd, Client(client_fd)));
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
					std::cout << "Client FD=" << fd << " déconnecté\n";
				}
				else
				{
					if (this->_clientBuffers[fd].getAllRead()) {
						Request req = Request(*this, fd);

						// Respond to client
						std::string reqstr = this->_clientBuffers[fd].getThatBody();

						req.parseRequest(reqstr, *this);
						std::string	reply = req.CreateResponse();
						send(fd, reply.c_str(), reply.size(), 0);
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
							this->_clientBuffers[fd] = Client(fd);
						}
						req.logRequest(*this);
					}
				}
			}
		}
	}
}