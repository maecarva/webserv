#pragma once

#include "Config.hpp"
#include "Client.hpp"

class Config;
class Client;

class Server
{
private:
	Config&	_config;
	
	// events / sockets
	int									_socketfd;
	int									_epoll_fd;
	struct epoll_event					_ev;
	std::map<int, Client >	_clientBuffers;

	std::map< std::string, unsigned int >				_tokenMap;
public:
	Server(Config& config);
	Server& operator=(const Server& server);
	~Server();


	void	handler();

	std::map< std::string, unsigned int >&	getTokenMap();
	Config&		getConfig();

	class ServerCreationError : public std::exception {
		virtual const char *what() const throw() {
			return "Can't create server.";
		}
	};
};
