#pragma once

#include "Config.hpp"

class Config;

class Route
{
private:
	std::string _name;
	std::vector<std::string> _allowed_methods;
	std::string _root;
	bool _autoindex;   // a set a true ou false des le debut
	std::string _index;
	std::map<int, std::string> _return;
	bool _uploads;
	std::vector<std::string> cgis_paths; // /usr/bin/python3 /bin/bash;               
    std::vector<std::string> cgi_ext; // .py .sh;   

public:

	Route();
	~Route();
	Route&	operator=(const Route& route);
	Route(const Route& route);
	// Name
	bool ParseServerConfigRouteName( const std::vector<std::string> &lineSplitted );

	// Allowed Methods
	bool isValidMethod( const std::string &method );
	void ParseServerConfigRouteAllow( std::vector<std::string> &lineSplitted );

	// root
	bool isValidRoot( const std::string &root );
	void ParseServerConfigRouteRoot( const std::vector<std::string> &lineSplitted );

	// Autoindex
	void ParseServerConfigRouteAutoindex( const std::vector<std::string> &lineSplitted );

	// Index
	void ParseServerConfigRouteIndex( const std::vector<std::string> &lineSplitted );

	// Return
	void ParseServerConfigRouteReturn( const std::vector<std::string> &lineSplitted );

	// Uploads
	void ParseServerConfigRouteUploads( const std::vector<std::string> &lineSplitted );


	std::string&				getName();
	std::vector<std::string>& 	getAllowedMethods();
	std::string					getRootDir();
	bool						getAutoIndex();
	std::string					getIndexFile();

	void						setRootDir(std::string str);

	std::map<int, std::string>	getReturn();
	bool						getUploads( void );

	void						printRoute();
};
