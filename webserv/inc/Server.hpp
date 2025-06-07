#ifndef SERVER
#define SERVER

#include <exception>
#include <map>
#include <vector>
#include <sys/epoll.h>

class Server
{
private:
	const char							*_label;
    unsigned short int					_port;
    const char							*_address;
	int									_socketfd;

	// events
	int									_epoll_fd;
	struct epoll_event					_ev;
	std::map<int, std::vector<char> >	_clientBuffers;

	// parsed config
	std::string							_indexfile;
	std::string							_rootdir;

	class ServerCreationError : public std::exception {
		virtual const char *what() const throw() {
			return "Can't create server.";
		}
	};


    Server();
public:
    Server(unsigned short int port, const char *address, const char *label);
    ~Server();

	void	handler();


	// getters
	const char		*getAddress() const;
	const char		*getLabel() const;
	int				getPort() const;
	std::string&	getRootDir();
	std::string&	getIndexFile();
};

#endif