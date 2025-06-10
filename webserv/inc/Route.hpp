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
	std::map<int, std::string> _return_code;

public:
	// Name
	void ParseServerConfigRouteName( const std::vector<std::string> &lineSplitted );

	// Allowed Methods
	bool isValidMethod( const std::string &method );
	void ParseServerConfigRouteAllow( const std::vector<std::string> &lineSplitted );

	// root
	bool isValidRoot( const std::string &root );
	void ParseServerConfigRouteRoot( const std::vector<std::string> &lineSplitted );

	// Autoindex
	void ParseServerConfigRouteAutoindex( const std::vector<std::string> &lineSplitted );

	// Index
	void ParseServerConfigRouteIndex( const std::vector<std::string> &lineSplitted );

	std::string&				getName();
	std::vector<std::string>& 	getAllowedMethods();
	std::string					getRootDir();
	bool						getAutoIndex();
	std::string					getIndexFile();
	std::map<int, std::string>	getErrorPages();

};
