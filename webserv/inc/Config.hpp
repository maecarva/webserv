#pragma once

#include "Webserv.hpp"
#include <fstream>
#include <vector> // vector
#include <string> // string
#include <map> // map
#include <netdb.h> // addrinfo
#include <cstdlib> // atoi()
#include <iostream>
#include <cstring>

#define BLANK_CHARACTERS " \t"
#define FORBIDDEN_NAME_CHARACTERS " \t\n\r\f\v/\\@!#$%^&*()=+[]{}|;:'\",<>?"
#define CLIENT_MAX_BODY_SIZE_O 1073741824 // 1 Go
#define CLIENT_MAX_BODY_SIZE_KO 1048576 // 1 Go / 1024
#define CLIENT_MAX_BODY_SIZE_MO 1024

struct Route
{
	std::vector<std::string> allowed_methods;
	std::string root;
	bool autoindex;   // a set a true ou false des le debut
	std::string index;



};

class Config
{
private:
	std::vector<std::string> _server_names;
	std::string _host;
	std::string _port;
	std::map<int, std::string> _error_pages;
	int _client_max_body_size;  // À set à -1
	std::map<std::string, Route> _routes;

// Getters
	std::vector<std::string> getServerNames( void ) const;
	std::vector< std::map<std::string, std::string> > getListen( void ) const;
	std::map<int, std::string> getErrorPages( void );
	int getClientMaxBodySize( void ) const;
	std::map<std::string, Route> getRoutes( void ) const;

// Parsing

// Listen
	bool isValidHostPort( const std::string &host, const std::string &port );
	void ParseServerConfigListen( const std::vector<std::string> &lineSplitted );

// Name
	bool isValidName( const std::string &name );
	void ParseServerConfigName( const std::vector<std::string> &lineSplitted );

// Error Pages
	bool isValidPage( const std::string &page );
	bool isValidErrorCode( const std::string &errorCode );
	bool isValidErrorCodePage( const std::string &errorCode, const std::string &page ); // On voit si les deux mis ensemble c'est bon
	void ParseServerConfigErrorPages( const std::vector<std::string> &lineSplitted );

// Client Max Body Size
	bool isValidClientMaxBodySize( const std::string &client_max_body_size );
	void ParseServerConfigClientMaxBodySize( const std::vector<std::string> &lineSplitted );

public:
// Default constructor and Destructor
	Config( void );
	~Config( void );

// Parse Config
	bool ParseServerConfig( std::ifstream &configFile );

};

// Useful Functions
std::vector<std::string> splitFromCharset( const std::string &str, const std::string &delimiters );
