#include "Config.hpp"

// Route
void Route::ParseServerConfigRouteName( const std::vector<std::string> &lineSplitted ) // revoir la fct
{
	_name = lineSplitted[1];
}

// Allowed Methods
bool Route::isValidMethod( const std::string &method )
{
	if ( method.find_first_of( FORBIDDEN_NAME_CHARACTERS ) != std::string::npos ) // Nom invalide
	{
		std::cerr << "Invalid route method: \'" << method << "\'." << std::endl;
		return ( false );
	}

	for ( size_t i = 0; i < _allowed_methods.size(); ++i ) // Nom deja existant
	{
		if ( _allowed_methods[i] == method )
		{
			std::cerr << "Route already has method: \'" << method << "\'." << std::endl;
			return ( false );
		}
	}

	return ( true );
}

void Route::ParseServerConfigRouteAllow( std::vector<std::string> &lineSplitted )
{
	for ( size_t i = 1; i < lineSplitted.size(); ++i )
	{
		std::transform( lineSplitted[i].begin(), lineSplitted[i].end(), lineSplitted[i].begin(), ::toupper );
		if ( isValidMethod( lineSplitted[i] ) )
			_allowed_methods.push_back( lineSplitted[i] );
	}
}


// Root
bool Route::isValidRoot( const std::string &root )
{
	struct stat info;

	if ( stat( root.c_str(), &info ) != 0 ) // Verifie si le dossier existe et s'il y'a un erreur d'accès.
	{
		std::cerr << "Root: \'" << root << "\' does not exist." << std::endl;
		return ( false );
	}

	if ( !( info.st_mode & S_IFDIR ) ) // Verifie si c'est un dossier
	{
		std::cerr << "Root: \'" << root << "\' is not a directory." << std::endl;
		return ( false ); // Ce n'est pas un dossier
	}

	if ( access( root.c_str(), R_OK | X_OK ) != 0 )  // Vérifie les droits d'accès (lecture + exécution)
	{
		std::cerr << "Root: \'" << root << "\' has an invalid access." << std::endl;
		return ( false );
	}

	return ( true );
}

void Route::ParseServerConfigRouteRoot( const std::vector<std::string> &lineSplitted )
{
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "Root: Invalid number of arguments." << std::endl;
		return ;
	}

	if ( this->isValidRoot( lineSplitted[1] ) )
	{
		_root = lineSplitted[1];
	}
}

// AutoIndex
void Route::ParseServerConfigRouteAutoindex( const std::vector<std::string> &lineSplitted )
{
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "Autoindex: Invalid number of arguments." << std::endl;
		return ;
	}

	if ( lineSplitted[1] == "on" )
		_autoindex = true;

	else if ( lineSplitted[1] == "off" )
		_autoindex = false;

	else
		std::cerr << "AutoIndex: \'" << lineSplitted[1] << "\' is Invalid." << std::endl;
}


// Index
void Route::ParseServerConfigRouteIndex( const std::vector<std::string> &lineSplitted )
{
	_index = lineSplitted[1];
}


// Parse Route
void Config::ParseServerConfigRoute( std::ifstream &configFile, std::string &line, std::vector<std::string> &lineSplitted )
{
	Route route;

	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "Route: Invalid number of arguments." << std::endl;
		// Skip tous les elements de la route
		return ;
	}

	route.ParseServerConfigRouteName( lineSplitted ); //! renvoyeru un booleen.

	while ( std::getline( configFile, line ) )
	{
		lineSplitted = splitFromCharset( line, BLANK_CHARACTERS );

		if ( lineSplitted.size() == 0 )
			break;

		if ( lineSplitted.size() == 1 )
		{
			std::cerr << "Invalid arguments after directive \'" << lineSplitted[0] << '\'' << std::endl;
			continue;
		}

		if ( lineSplitted[0] == "allow" )
			route.ParseServerConfigRouteAllow( lineSplitted );

		else if ( lineSplitted[0] == "root" )
			route.ParseServerConfigRouteRoot( lineSplitted );

		else if ( lineSplitted[0] == "autoindex" )
			route.ParseServerConfigRouteAutoindex( lineSplitted );

		else if ( lineSplitted[0] == "index" )
			route.ParseServerConfigRouteIndex( lineSplitted );
	}
	_routes.push_back( route );
}


// * GETTERS
std::string&				Route::getName() {
	return this->_name;
};

std::vector<std::string>& 	Route::getAllowedMethods() {
	return this->_allowed_methods;
};

std::string					Route::getRootDir() {
	return this->_root;
};

bool						Route::getAutoIndex() {
	return this->_autoindex;
};

std::string					Route::getIndexFile() {
	return this->_index;
};

std::map<int, std::string>	Route::getErrorPages() {
	return this->_return_code;
};


void	Route::printRoute() {
	PRINTCLN(MAG, "Path:");
	std::cout << this->getName() << std::endl;

	PRINTCLN(MAG, "Allowed methods:");
	for (std::vector<std::string>::iterator it = this->_allowed_methods.begin(); it != this->_allowed_methods.end(); it++)
	{
		std::cout << (*it) SPACE;
	}
	NEWLINE;

	PRINTCLN(MAG, "Root directory:");
	std::cout << this->getRootDir() << std::endl;
	
	PRINTC(MAG, "Auto Index: ");
	std::cout << (this->getAutoIndex() == true ? "true" : "false") << std::endl;

	PRINTCLN(MAG, "Index file:");
	std::cout << this->getIndexFile() << std::endl;
	PRINTCLN(MAG, "Error pages:");

	for (std::map<int, std::string>::iterator it = this->_return_code.begin(); it != this->_return_code.end(); it++)
	{
		std::cout << "\t" << (*it).first SPACE << (*it).second << std::endl;
	}
	
}
