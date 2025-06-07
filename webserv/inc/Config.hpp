#include <vector> // vector
#include <string> // string
#include <map> // map
#include <netdb.h> // addrinfo
#include <cstdlib> // atoi()

#define BLANK_CHARACTERS " \t"
#define FORBIDDEN_NAME_CHARACTERS " \t\n\r\f\v/\\@!#$%^&*()=+[]{}|;:'\",<>?"
#define CLIENT_MAX_BODY_SIZE_O 1073741824 // 1 Go
#define CLIENT_MAX_BODY_SIZE_KO 1048576 // 1 Go / 1024
#define CLIENT_MAX_BODY_SIZE_MO 1024


struct RouteConfig
{
	std::vector<std::string> _methods;
	std::string _root;



};

class Config
{
private:
	std::vector<std::string> _server_names;
	std::vector< std::map<std::string, std::string> > _listen;
	std::map<int, std::string> _error_pages;
	int _client_max_body_size;  // À set à -1
	std::map<std::string, RouteConfig> _routes;

public:
	Config( void );
	~Config( void );

	std::vector<std::string> getServerNames( void ) const;
	std::vector< std::map<std::string, std::string> > getListen( void ) const;
	std::map<int, std::string> getErrorPages( void );
	int getClientMaxBodySize( void ) const;
	std::map<std::string, Route> getRoutes( void ) const;

};
