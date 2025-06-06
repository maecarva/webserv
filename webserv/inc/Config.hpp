#include <vector> // vector
#include <string> // string
#include <map> // map
#include <netdb.h> // addrinfo
#include <cstdlib> // atoi()

#define BLANK_CHARACTERS " \t"
#define FORBIDDEN_NAME_CHARACTERS " \t\n\r\f\v/\\@!#$%^&*()=+[]{}|;:'\",<>?"

class Config
{
private:
	std::vector<std::string> _server_names;
	std::vector< std::map<std::string, std::string> > _listen;
	std::map<int, std::string> _error_pages;

};
