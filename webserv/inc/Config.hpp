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
	std::vector<std::string> allowed_methods;
	std::string root;




};

class Config
{
private:
	std::vector<std::string> _server_names;
	std::vector< std::map<std::string, std::string> > _listen;
	std::map<int, std::string> _error_pages;
	int _client_max_body_size;  // À set à -1
	std::map<std::string, RouteConfig> _routes;

// Getters
	std::vector<std::string> getServerNames( void ) const;
	std::vector< std::map<std::string, std::string> > getListen( void ) const;
	std::map<int, std::string> getErrorPages( void );
	int getClientMaxBodySize( void ) const;
	std::map<std::string, Route> getRoutes( void ) const;

// Parsing

// Listen
	bool Config::isValidHostPort( const std::string &host, const std::string &port )
	void Config::ParseServerConfigListen( const std::vector<std::string> &lineSplitted )

// Name
	bool Config::isValidName( const std::string &name )
	void Config::ParseServerConfigName( const std::vector<std::string> &lineSplitted )

// Error Pages
	bool Config::isValidPage( const std::string &page )
	bool Config::isValidErrorCode( const std::string &errorCode )
	bool Config::isValidErrorCodePage( const std::string &errorCode, const std::string &page ) // On voit si les deux mis ensemble c'est bon
	void Config::ParseServerConfigErrorPages( const std::vector<std::string> &lineSplitted )

// Client Max Body Size
	bool Config::isValidClientMaxBodySize( const std::string &client_max_body_size )
	void ParseServerConfigClientMaxBodySize( const std::vector<std::string> &lineSplitted )

public:
// Default constructor and Destructor
	Config( void );
	~Config( void );

// Parse Config
	bool ParseServerConfig( std::ifstream &configFile )

};

// Useful Functions
std::vector<std::string> splitFromCharset( const std::string &str, const std::string &delimiters )
