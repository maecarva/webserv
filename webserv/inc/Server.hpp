#ifndef SERVER
#define SERVER


#include "Webserv.hpp"

class Config;

class Server
{
private:
	// Config								_config;

	// events / sockets
	int									_socketfd;
	int									_epoll_fd;
	struct epoll_event					_ev;
	std::map<int, std::vector<char> >	_clientBuffers;

	class ServerCreationError : public std::exception {
		virtual const char *what() const throw() {
			return "Can't create server.";
		}
	};


    Server();
public:
    Server(Config& config);
	Server& operator=(const Server&);
    ~Server();

	void	handler();


	// getters
	Config&			getConfig();
	const char		*getAddress() const;
	const char		*getLabel() const;
	int				getPort() const;
	std::string&	getRootDir();
	std::string&	getIndexFile();
	bool			getDirectoryListing();
};

#endif