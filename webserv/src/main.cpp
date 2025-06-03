#include "Webserv.hpp"


int setNonBlocking(int fd)
{
	// fcntl allow manipulation of fd
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK); // set fd to be non blocking
}

// socket = bidirectionnal fd
// setsockopt = change socket behavior, here we use ipv4 address and allow reuse after

void    server() {
    int		listenfd = -1;
    struct	sockaddr_in addr;
	int		opt = 1;
	int		epoll_fd = -1;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::perror("Socket creation failed");
		return ;
	}

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::perror("setsockopt failed");
		close(listenfd);
		return ;
	}

	if (setNonBlocking(listenfd) < 0)
	{
		std::perror("setNonBlocking failed");
		return ;
	}

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; // IPv4
	addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
	addr.sin_port = htons(PORT);

	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { // link listenfd to addr
        std::perror("bind failed");
        close(listenfd);
    }

	if (listen(listenfd, 10) < 0) { // start listening
        std::perror("listen failed");
        close(listenfd);
    }
	
	epoll_fd = epoll_create(1); // create epoll instance
	struct epoll_event ev;
	std::memset(&ev, 0, sizeof(ev));

	ev.events = EPOLLIN;
	ev.data.fd = listenfd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &ev) < 0) {
		std::perror("epoll_ctl failed");
		close(listenfd);
		close(epoll_fd);
		return ;
	}

	std::map<int, std::vector<char> > clientBuffers;

	while (true) {
		struct epoll_event	events[MAX_EVENTS];
		int					nready = epoll_wait(epoll_fd, events, MAX_EVENTS, -1); // return number of events to handle
		if (nready < 0) 
		{
			if (errno == EINTR)
				continue ;
			std::perror("epoll_wait failed");
			break ;
		}


		for (int i = 0; i < nready; i++) // loop for each events
		{
			int fd = events[i].data.fd;
			/// -------
			if (fd == listenfd) // first case : new connection
			{
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_fd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
				if ( client_fd < 0) {
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						break ;
					else {
						std::perror("accept client failed");
						break ;
					}
				}

				if (setNonBlocking(client_fd) < 0)
				{
					std::perror("setNonblicking client_fd");
					close(client_fd);
					continue ;
				}

				struct epoll_event client_ev;
				std::memset(&client_ev, 0, sizeof(client_ev));
				client_ev.events = EPOLLIN | EPOLLET;
				client_ev.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) < 0)
				{
					std::perror("add client fd to epoll failed");
					close(client_fd);
					continue ;
				}

				clientBuffers[client_fd] = std::vector<char>();
                    std::cout << "Nouvelle connexion: FD=" << client_fd
                              << " depuis " << inet_ntoa(client_addr.sin_addr)
                              << ":" << ntohs(client_addr.sin_port) << "\n";
			}
			/// -------
			else // second case : client socket ready for read
			{
				if (events[i].events & EPOLLIN)
				{
					bool closed = false;
					while (true)
					{
						char	buf[BUFSIZ];
						ssize_t	count = recv(fd, buf, sizeof(buf), 0);
						if (count < 0) {
							if (errno == EAGAIN || errno == EWOULDBLOCK)
								break ;
							else
							{
								std::perror("recv failed");
								closed = true;
								break ;
							}
						}
						else if (count == 0)
						{
							closed = true;
							break ;
						}
						else // handle bytes read
						{
							clientBuffers[fd].insert(clientBuffers[fd].end(), buf, buf + count);
							//std::cout << "Reçu sur FD=" << fd << ": " << std::string(buf, buf + count) << "\n";
						}
					}

					if (closed)
					{
					    // Déconnexion du client : retirer de epoll et fermer
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
                            std::perror("epoll_ctl DEL");
                        close(fd);
                        clientBuffers.erase(fd);
                        std::cout << "Client FD=" << fd << " déconnecté\n";	
					}
					else
					{
						std::string reqstr;
						std::vector<char>::iterator it = clientBuffers[fd].begin();
						while (it != clientBuffers[fd].end())
						{
							reqstr.push_back(*it);
							it++;
						}
						Request req = Request(reqstr.c_str());

						// (Optionnel) Vous pouvez décider de répondre au client ici
                        // Par exemple, renvoyer un message “Bonjour” puis fermer


						const char reply[] = "HTTP/1.1 200 OK\r\nContent-Length: 226\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>S.C.E.P</title><img alt=SCEP src=https://m.media-amazon.com/images/I/71s4e9komjL._AC_UF1000,1000_QL80_.jpg>";
                        send(fd, reply, sizeof(reply) - 1, 0);
                        // Ensuite, on ferme la connexion (serveur « one-shot »)
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
                            std::perror("epoll_ctl DEL après send");
                        close(fd);
                        clientBuffers.erase(fd);
                        std::cout << "Réponse envoyée\n";
					}
				}
			}
		}
		
	}
	close(listenfd);
	close(epoll_fd);
}

void    client() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1000] = {0};
    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return ;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return ;
    }
    // Connect to the server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return ;
    }
    std::string hello = "Hello from client";
    send(sock, hello.c_str(), hello.size(), 0);
    std::cout << "Hello message sent" << std::endl;
    read(sock, buffer, 1000);
    std::cout << "Received: " << buffer << std::endl;
    // Close the socket
    close(sock);
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid args" << std::endl;
        return 1;
    }
    if (string(argv[1]) == "server")
        server();
    else
        client();
    return (0 * argc * argv[0][0]);
}
