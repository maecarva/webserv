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
#include "Route.hpp"

#define BLANK_CHARACTERS " \t"
#define FORBIDDEN_NAME_CHARACTERS " \t\n\r\f\v/\\@!#$%^&*()=+[]{}|;:'\",<>?" // Pas de . car www.google.fr est valide
#define CLIENT_MAX_BODY_SIZE_O 1073741824 // 1 Go
#define CLIENT_MAX_BODY_SIZE_KO 1048576 // 1 Go / 1024
#define CLIENT_MAX_BODY_SIZE_MO 1024

class Route;

class Config
{
private:
	std::vector<std::string> _server_names;
	std::string _host;
	std::string _port;
	std::map<int, std::string> _error_pages;
	int _client_max_body_size;  // À set à -1
	std::vector<Route> _routes;



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
	void ParseServerConfigErrorPages( const std::vector<std::string> &lineSplitted );

// Client Max Body Size
	bool isValidClientMaxBodySize( const std::string &client_max_body_size );
	void ParseServerConfigClientMaxBodySize( const std::vector<std::string> &lineSplitted );

// Route
	void ParseServerConfigRoute( std::ifstream &configFile, std::string &line, std::vector<std::string> &lineSplitted );

// Uploads
	void ParseServerConfigRouteUploads( const std::vector<std::string> &lineSplitted );

public:
// Default constructor and Destructor
	Config( void );
	~Config( void );

// Parse Config
	bool ParseServerConfig( std::ifstream &configFile );
	static void PrintConfig(std::vector<Config>& configs);


	// Getters
	std::vector<std::string> getServerNames( void ) const;
	std::string getListen( void ) const;
	std::string getServerHost( void ) const;
	std::string getServerPort( void ) const;
	std::map<int, std::string> getErrorPages( void );
	int getClientMaxBodySize( void ) const;
	std::vector<Route> getRoutes( void ) const;
};

// Useful Functions
std::vector<std::string> splitFromCharset( const std::string &str, const std::string &delimiters );

std::vector<Config> CreateConfigs( std::ifstream &configFile );
