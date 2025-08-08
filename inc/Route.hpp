#pragma once

#include "Config.hpp"

class Config;

class Route
{
private:
	std::string 										_name;
	std::vector<std::string> 							_allowed_methods;
	std::string 										_root;
	bool 												_autoindex;   // a set a true ou false des le debut // directory listing
	std::string 										_index;
	bool												_indexFile;
	bool												_redirect;
	std::string 										_return;
	bool 												_uploads;
	std::string											_uploadfolder;
	bool												_iscgi;
	std::map<std::string, std::string >					_cgi;
			// .py /usr/bin/python3
	bool												_guard;
	std::string											_guardPage;
	std::string											_protection;

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

// Cgi
	bool isValidExtension( const std::string &extension );
	bool isValidCommand( const std::string &command );
	void ParseServerConfigRouteCgi( const std::vector<std::string> &lineSplitted );

// Guard
	void ParseServerConfigGuard( const std::vector<std::string> &lineSplitted );

// Protection
	void ParseServerConfigProtection( const std::vector<std::string> &lineSplitted );

	std::string&				getName();
	std::vector<std::string>& 	getAllowedMethods();
	std::string					getRootDir();
	bool						getAutoIndex();
	std::string					getIndexFile();
	bool						getDirectoryListing();
	std::string					getUploadDir();
	std::map<std::string, std::string > getValidsCGI();
	bool getGuard( void ) const;
	std::string getGuardPage( void ) const;
	std::string getProtection();



	void						setRootDir(std::string str);
 
	bool						isRedirect();
	bool						isCGI();
	std::string					getReturn();
	bool						getUploads( void );

	void						printRoute();

	bool						indexFileIsSet();

};
