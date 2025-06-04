#include "Server.hpp"
#include "Webserv.hpp"

Server::Server() {};

Server::Server(unsigned short int	port, const char	*address, const char *label) {

	this->_port = port;
	this->_address = address;
	this->_label = label;

	int		listenfd = -1;
    struct	sockaddr_in addr;
	int		opt = 1;
	int		epoll_fd = -1;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::perror("Socket creation failed");
		throw Server::ServerCreationError();
	}

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

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; // IPv4
	addr.sin_addr.s_addr = inet_addr(this->_address); // interface to listen 127.0.0.1
	addr.sin_port = htons(this->_port);

	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { // link listenfd to addr
        std::perror("bind failed");
        close(listenfd);
		throw Server::ServerCreationError();
    }

	if (listen(listenfd, 10) < 0) { // start listening
        std::perror("listen failed");
        close(listenfd);
		throw Server::ServerCreationError();
    }
	
	std::ostringstream oss;
	oss << "Server '" << this->_label << "' listening on " << this->_address << ":" << this->_port;
	Logger::debug(oss.str().c_str());

	epoll_fd = epoll_create(1); // create epoll instance
	struct epoll_event ev;
	std::memset(&ev, 0, sizeof(ev));

	ev.events = EPOLLIN;
	ev.data.fd = listenfd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &ev) < 0) {
		std::perror("epoll_ctl failed");
		close(listenfd);
		close(epoll_fd);
		throw Server::ServerCreationError();
		return ;
	}

	std::memset(&this->_ev, 0, sizeof(this->_ev));

	this->_epoll_fd = epoll_fd;
	this->_socketfd = listenfd;
};

void	Server::handler() {

	struct epoll_event	events[MAX_EVENTS];
	int					nready = epoll_wait(this->_epoll_fd, events, MAX_EVENTS, 0); // return number of events to handle, 0 to prevent blocking for connexion
	if (nready < 0)
	{
		std::perror("epoll_wait failed");
		return ;
	}

	for (int i = 0; i < nready; i++) // loop for each events
	{
		int fd = events[i].data.fd;
		/// -------
		if (fd == this->_socketfd) // first case : new connection
		{
			struct sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			int client_fd = accept(this->_socketfd, (struct sockaddr*)&client_addr, &client_len);
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
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) < 0)
			{
				std::perror("add client fd to epoll failed");
				close(client_fd);
				continue ;
			}

			this->_clientBuffers[client_fd] = std::vector<char>();
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
						this->_clientBuffers[fd].insert(this->_clientBuffers[fd].end(), buf, buf + count);
						//std::cout << "Reçu sur FD=" << fd << ": " << std::string(buf, buf + count) << "\n";
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

					// Respond to client
					std::string reqstr;
					std::vector<char>::iterator it = this->_clientBuffers[fd].begin();
					while (it != this->_clientBuffers[fd].end())
					{
						reqstr.push_back(*it);
						it++;
					}
					Request req = Request(reqstr.c_str());


					const char reply[] = "HTTP/1.1 200 OK\r\nContent-Length: 226\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>S.C.E.P</title><img alt=SCEP src=https://m.media-amazon.com/images/I/71s4e9komjL._AC_UF1000,1000_QL80_.jpg>";
					send(fd, reply, sizeof(reply) - 1, 0);
					// Close connection
					if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
						std::perror("epoll_ctl DEL après send");
					close(fd);
					this->_clientBuffers.erase(fd);
					std::cout << "Réponse envoyée\n";
				}
			}
		}
	}
}

Server::~Server() {
	close(this->_epoll_fd);
	close(this->_socketfd);
};


const char		*Server::getAddress() const {
	return this->_address;
};

const char		*Server::getLabel() const {
	return this->_label;
};

int				Server::getPort() const {
	return this->_port;
};