#include "Config.hpp"

// Default constructor and Destructor
Route::Route( void ) : _autoindex( false ), _uploads( false )
{
	Logger::debug("Creating Route");
}

Route::~Route( void ) {}


Route&	Route::operator=(const Route& route) {
	if (this != &route) {
		_name = route._name;
		_allowed_methods = route._allowed_methods;
		_root = route._root;
		_autoindex = route._autoindex;
		_index = route._index;
		_return = route._return;
		_uploads = route._uploads;
		cgis_paths = route.cgis_paths;
		cgi_ext = route.cgi_ext;
	}
	return *this;
}

Route::Route(const Route& route) : _name(route._name), _allowed_methods(route._allowed_methods), _root(route._root)
		,_autoindex(route._autoindex)
		,_index(route._index)
		,_return(route._return)
		,_uploads(route._uploads)
		,cgis_paths(route.cgis_paths)
		,cgi_ext(route.cgi_ext)
{}

// Route
bool Route::ParseServerConfigRouteName( const std::vector<std::string> &lineSplitted ) // revoir la fct
{
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "locate: Invalid number of arguments." << std::endl;
		return ( false );
	}

	// if ( lineSplitted[1].find_first_of( FORBIDDEN_NAME_CHARACTERS ) != std::string::npos ) // Nom invalide
	// {
	// 	std::cerr << "Invalid route name: \'" << lineSplitted[1] << "\'." << std::endl;
	// 	return ( false );
	// }

	_name = lineSplitted[1];

	return ( true );
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
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "Index: Invalid number of arguments." << std::endl;
		return ;
	}

	// if ( access( lineSplitted[1].c_str(), R_OK | X_OK ) != 0 )  // Vérifie les droits d'accès (lecture + exécution)
	// {
	// 	std::perror( lineSplitted[1].c_str() );
	// 	return ;
	// }

	_index = lineSplitted[1];
}


// Return
void Route::ParseServerConfigRouteReturn( const std::vector<std::string> &lineSplitted )
{
	if ( lineSplitted.size() != 3 )
	{
		std::cerr << "Return: Invalid number of arguments." << std::endl;
		return ;
	}

	if ( lineSplitted[1].size() != 3 )
	{
		std::cerr << "Invalid return code \'" <<  lineSplitted[1] << "\'." << std::endl;
		return ;
	}

	char *end;
	long returnCode = strtol( lineSplitted[1].c_str(), &end, 10 );

	if ( *end != '\0' )
	{
		std::cerr << "Invalid return code \'" <<  lineSplitted[1] << "\'." << std::endl;
		return ;
	}

	if ( returnCode < 0 || returnCode > 599 )
	{
		std::cerr << "Invalid error_code \'" << returnCode << "\'." << std::endl;
		return ;
	}

	_return[( int ) returnCode] = lineSplitted[2];
}


// Uploads
void Route::ParseServerConfigRouteUploads( const std::vector<std::string> &lineSplitted )
{
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "uploads: Invalid number of arguments." << std::endl;
		return ;
	}

	if ( lineSplitted[1] == "on" )
		_uploads = true;

	else if ( lineSplitted[1] == "off" )
		_uploads = false;

	else
		std::cerr << "Uploads: \'" << lineSplitted[1] << "\' is Invalid." << std::endl;
}


// Parse Route
void Config::ParseServerConfigRoute( std::ifstream &configFile, std::string &line, std::vector<std::string> &lineSplitted )
{
	Route route;

	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "Location: Invalid number of arguments." << std::endl;
		// Skip tous les elements de la route
		return ;
	}

	if ( !route.ParseServerConfigRouteName( lineSplitted ) ) //! renvoyer un booleen.
	{
		return ;
	}

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
		
		else if ( lineSplitted[0] == "return" )
			route.ParseServerConfigRouteReturn( lineSplitted );

		else if ( lineSplitted[0] == "uploads" )
			route.ParseServerConfigRouteUploads( lineSplitted );

		else
		{
			std::cerr << "Invalid Route directive \'" << line << "\'." << std::endl;
			break;
		}
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

std::map<int, std::string>	Route::getReturn() {
	return this->_return;
};

bool Route::getUploads( void ) { return ( _uploads ); }


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
	PRINTCLN(MAG, "Return:");

	for (std::map<int, std::string>::iterator it = this->_return.begin(); it != this->_return.end(); it++)
	{
		std::cout << "\t" << (*it).first SPACE << (*it).second << std::endl;
	}

	PRINTCLN(MAG, "Uploads");
	std::cout << (this->getUploads() == true ? "true" : "false") << std::endl;
	
}


// * setters

void Route::setRootDir(std::string str) {
	this->_root = str;
}
