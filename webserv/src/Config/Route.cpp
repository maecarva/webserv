#include "Config.hpp"

// Default constructor and Destructor
Route::Route( void ) : _autoindex( false ), _redirect( false ), _uploads( false ), _iscgi (false), _directory_listing( false )
{}

Route::~Route( void ) {}

Route&	Route::operator=(const Route& route) {
	if (this != &route) {
		_name = route._name;
		_allowed_methods = route._allowed_methods;
		_root = route._root;
		_autoindex = route._autoindex;
		_index = route._index;
		_redirect = route._redirect;
		_return = route._return;
		_uploads = route._uploads;
		_uploadfolder = route._uploadfolder;
		_iscgi = route._iscgi;
		_cgi = route._cgi;
		_directory_listing = route._directory_listing;
	}
	return *this;
}

Route::Route(const Route& route) : _name(route._name), _allowed_methods(route._allowed_methods), _root(route._root)
		,_autoindex(route._autoindex)
		,_index(route._index)
		,_redirect(route._redirect)
		,_return(route._return)
		,_uploads(route._uploads)
		,_uploadfolder(route._uploadfolder)
		,_iscgi(route._iscgi)
		,_cgi(route._cgi)
		,_directory_listing(route._directory_listing)
{}

// Route
bool Route::ParseServerConfigRouteName( const std::vector<std::string> &lineSplitted ) // revoir la fct
{
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "locate: Invalid number of arguments." << std::endl;
		throw parsingError();
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
		throw parsingError();
		return ( false );
	}

	for ( size_t i = 0; i < _allowed_methods.size(); ++i ) // Nom deja existant
	{
		if ( _allowed_methods[i] == method )
		{
			std::cerr << "Route already has method: \'" << method << "\'." << std::endl;
			throw parsingError();
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
		throw parsingError();
		return ( false );
	}

	if ( !( info.st_mode & S_IFDIR ) ) // Verifie si c'est un dossier
	{
		std::cerr << "Root: \'" << root << "\' is not a directory." << std::endl;
		throw parsingError();
		return ( false ); // Ce n'est pas un dossier
	}

	// if ( access( root.c_str(), R_OK | X_OK ) != 0 )  // Vérifie les droits d'accès (lecture + exécution)
	// {
	// 	std::cerr << "Root: \'" << root << "\' has an invalid access." << std::endl;
	// 	return ( false );
	// }

	return ( true );
}

void Route::ParseServerConfigRouteRoot( const std::vector<std::string> &lineSplitted )
{
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "Root: Invalid number of arguments." << std::endl;
		throw parsingError();
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
		throw parsingError();
		return ;
	}

	if ( lineSplitted[1] == "on" )
		_autoindex = true;

	else if ( lineSplitted[1] == "off" )
		_autoindex = false;

	else
	{
		std::cerr << "AutoIndex: \'" << lineSplitted[1] << "\' is Invalid." << std::endl;
		throw parsingError();
	}
}

// Index
void Route::ParseServerConfigRouteIndex( const std::vector<std::string> &lineSplitted )
{
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "Index: Invalid number of arguments." << std::endl;
		throw parsingError();
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
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "Return: Invalid number of arguments." << std::endl;
		throw parsingError();
		return ;
	}
	_redirect = true;
	_return = lineSplitted[1];
}


// Uploads
void Route::ParseServerConfigRouteUploads( const std::vector<std::string> &lineSplitted )
{
	// std::cout << lineSplitted[0] SPACE << lineSplitted[1] << std::endl;
	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "uploads: Invalid number of arguments." << std::endl;
		throw parsingError();
		return ;
	}

	// if (!is_directory(lineSplitted[1].c_str()))
	// 	std::cerr << "Error: " << lineSplitted[1] << " is not a directory" << std::endl;
	_uploadfolder = lineSplitted[1];
	_uploads = true;
}


// Cgi
bool Route::isValidExtension( const std::string &extension )
{
	if ( extension.empty() || extension[0] != '.' || extension.size() < 2 )
		return ( false );

	return ( true );
}

bool Route::isValidCommand( const std::string &command )
{
	if ( access( command.c_str(), F_OK | X_OK ) != 0 )
		return ( false );

	return ( true );
}

void Route::ParseServerConfigRouteCgi( const std::vector<std::string> &lineSplitted )
{

	// for (size_t i = 0; i < lineSplitted.size(); i++)
	// {
	// 	std::cout << "linesplitted " << i << " : " << lineSplitted[i] << std::endl;
	// }
	

	if ( lineSplitted.size() < 3 )
	{
		std::cerr << "cgi: Invalid number of arguments." << std::endl;
		throw parsingError();
		return ;
	}

	if ( _cgi.find( lineSplitted[1] ) != _cgi.end() )
	{
		std::cerr << "cgi: Extension already exists" << std::endl;
		throw parsingError();
		return ;
	}

	std::string extension = lineSplitted[1];

	if ( !this->isValidExtension( extension ) )
	{
		std::cerr << "cgi: Invalid extension: " << extension << std::endl;
		throw parsingError();
		return;
	}

	std::string command = lineSplitted[2];

	if ( !this->isValidCommand( command ) )
	{
		std::cerr << "cgi: Invalid command: " << command << std::endl;
		throw parsingError();
		return;
	}

	_cgi[extension] = command;
	std::cout << extension << ":" << command << std::endl;
	_iscgi = true;
}


// Parse Route
void Config::ParseServerConfigRoute( std::ifstream &configFile, std::string &line, std::vector<std::string> &lineSplitted )
{
	Route route;

	if ( lineSplitted.size() != 2 )
	{
		std::cerr << "Location: Invalid number of arguments." << std::endl;
		throw parsingError();
		// Skip tous les elements de la route
		return ;
	}

	if ( !route.ParseServerConfigRouteName( lineSplitted ) ) //! renvoyer un booleen.
	{
		std::cerr << "Invalid Route Name." << std::endl;
		throw parsingError();
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
			throw parsingError();
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

		else if ( lineSplitted[0] == "cgi_extension" )
			route.ParseServerConfigRouteCgi( lineSplitted );

		else
		{
			std::cerr << "Invalid Route directive \'" << line << "\'." << std::endl;
			throw parsingError();
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


bool						Route::getDirectoryListing() {
	return this->_directory_listing;
}

std::string					Route::getReturn() {
	return this->_return;
};

bool Route::getUploads( void ) { 
	return ( this->_uploads );
}

bool						Route::isRedirect() {
	return _redirect;
}

bool						Route::isCGI() {
	return _iscgi;
}

std::string		Route::getUploadDir() {
	return this->_uploadfolder;
}


std::map<std::string, std::string > Route::getValidsCGI() {
	return this->_cgi;
}

void	Route::printRoute() {

	PRINTCLN(MAG, "\n\nPath:");
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
	
	PRINTCLN(MAG, "Redirection:");
	std::cout << this->getReturn() << std::endl;

	PRINTCLN(MAG, "Uploads");
	if (this->getUploads())
		std::cout << this->getUploadDir() << std::endl;
	else
		std::cout << "false" << std::endl;

	PRINTCLN(MAG, "Cgi:");
	for ( std::map< std::string, std::string>::iterator it = _cgi.begin(); it != _cgi.end(); ++it )
	{
		std::cout << "extention: " << (*it).first << " path : " << (*it).second << std::endl;
	}
	std::cout << std::endl;
}

// * setters

void Route::setRootDir(std::string str) {
	this->_root = str;
}
