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
	int nready = epoll_wait(this->_epoll_fd, events, MAX_EVENTS, 0);
	if (nready < 0) {
		std::perror("epoll_wait failed");
		return;
	}

	for (int i = 0; i < nready; i++) {
		int fd = events[i].data.fd;
		uint32_t ev = events[i].events;

		// new client
		if (fd == this->_socketfd) {
			sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			int client_fd = accept(this->_socketfd,
								   (sockaddr*)&client_addr,
								   &client_len);
			if (client_fd < 0) {
				if (errno != EAGAIN && errno != EWOULDBLOCK)
					std::perror("accept failed");
				continue;
			}

			setNonBlocking(client_fd);
			epoll_event client_ev;
			client_ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
			client_ev.data.fd = client_fd;
			epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD,
					  client_fd, &client_ev);

			this->_clientBuffers.insert(std::make_pair(client_fd, Client(client_fd, this->getConfig().getClientMaxBodySize())));
		
		}
			// known client
		else {
			bool closed = false;
			bool bodyComplete = false;

			if (ev & (EPOLLERR | EPOLLHUP)) {
				closed = true;
			}
			else if (ev & EPOLLIN) {
				while (true) {
					char buf[BUFSIZ];
					ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
					if (n > 0) {
						buf[n] = '\0';
						std::string bufstr(buf, n);


						Client& client = _clientBuffers[fd];
						if (client.getContentLength() == 0) {
							size_t pos = bufstr.find("Content-Length:");
							if (pos == std::string::npos)
								pos = bufstr.find("content-length:");
							if (pos != std::string::npos) {
								size_t start = bufstr.find(':', pos) + 1;
								while (start < bufstr.size() && isspace(bufstr[start]))
									++start;
								long len = strtol(bufstr.c_str() + start, NULL, 10);
								client.setContentLength(len);
							}
						}

						client.addBodyCount(bufstr.c_str(), bufstr.size());

						if (client.getAllRead()) {
							bodyComplete = true;
							break;
						}
						continue;
					}
					else if (n == 0) {
						closed = true;
					}
					break;
				}
			}

			if (closed) {
				epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
				_clientBuffers.erase(fd);
				std::ostringstream oss;
				oss << "Client FD=" << fd << " déconnecté";
				Logger::debug(oss.str().c_str());
			}
			else if (bodyComplete || _clientBuffers[fd].getAllRead())
			{
				Client& client = _clientBuffers[fd];
				std::string body = client.getThatBody();

				Request req(*this, fd);
				req.parseRequest(body, *this);
				std::string reply = req.CreateResponse();
				ssize_t		offset = 0;
				ssize_t		sent = 0;

				while (( size_t )offset < reply.size()) {
					sent = send(fd, reply.c_str() + offset, reply.size() - offset, 0);
					if (sent > 0) {
						offset += sent;
					}
				}

				if (!req.isKeepAlive()) {
					epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
					close(fd);
					_clientBuffers.erase(fd);
				}
				else
					_clientBuffers[fd] = Client(fd, getConfig().getClientMaxBodySize());
				req.logRequest(*this);
			}
		}
	}
}


std::map< std::string, unsigned int >&	Server::getTokenMap() {
	return this->_tokenMap;
};