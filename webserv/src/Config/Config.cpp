#include "Config.hpp"

// Default constructor and Destructor
Config::Config( void )
{
	std::cout << "Creating Server Config" << std::endl;
}

Config::~Config( void ) {}


// Useful Functions
std::vector<std::string> splitFromCharset( const std::string &str, const std::string &delimiters )
{
	std::vector<std::string> tokens;
	size_t end, start = 0;

	while ( start < str.size() )
	{
		while ( start < str.size() && ( delimiters.find( line[start] ) != std::string::npos ) )
			++start;

		if ( start >= str.size() )
			break;

		end = start;
		while ( end < str.size() && !( delimiters.find( line[end] ) != std::string::npos ) )
			++end;

		tokens.push_back( substr() );
		start = end;
	}

	return ( tokens );
}


// Listen
bool Config::isValidHostPort( const std::string &host, const std::string &port )
{
	struct addrinfo hints, *res = NULL;
	int err;

	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;           // IPv4 ou IPv6
	hints.ai_socktype = SOCK_STREAM;

	err = getaddrinfo( host.c_str(), port.c_str(), &hints, &res );
	if ( err != 0 )
	{
		std::cerr << "getaddrinfo error: " << gai_strerror(err) << '.' << std::endl;
		return ( false );
	}
	freeaddrinfo( res );

	return ( true );
}

void Config::ParseServerConfigListen( const std::vector<std::string> &lineSplitted )
{
	size_t colon;
	std::string host;
	std::string port;
	std::map<std::string, std::string> entry;

	for ( size_t i = 1; i < lineSplitted.size(); ++i )
	{
		colon = lineSplitted[i].find(':');
		host = ( colon != std::string::npos ) ? lineSplitted[i].substr( 0, colon ) : "0.0.0.0";
		port = ( colon != std::string::npos ) ? lineSplitted[i].substr( colon + 1 ) : lineSplitted[i];
		if ( this->isValidHostPort( host, port ) )
		{
			data["host"] = host;
			data["port"] = port;
			_listen.push_back( entry );
		}
	}
}


// Name
bool Config::isValidName( const std::string &name )
{
	if ( line.find_first_of( FORBIDDEN_NAME_CHARACTERS ) != std::string::npos ) // Nom invalide
	{
		std::cerr << "Invalid server name: \'" << lineSplitted[i] << "\'." << std::endl;
		return ( false );
	}

	for ( size_t i = 0; i < _name.size(); ++i ) // Nom deja existant
	{
		if ( _name[i] == name )
		{
			std::cerr << "Server already has name: \'" << lineSplitted[i] << "\'." << std::endl;
			return ( false );
		}
	}

	return ( true );
}

void Config::ParseServerConfigName( const std::vector<std::string> &lineSplitted )
{
	for ( size_t i = 1; i < lineSplitted.size(); ++i )
	{
		if ( this->isValidName( lineSplitted[i] ) )
			_server_names.push_back( lineSplitted[i] );
	}
}


// Error Pages
bool Config::isValidPage( const std::string &page )
{
	return ( true );
}

bool Config::isValidErrorCode( const std::string &errorCode )
{
	if ( errorCode.size() != 3 )
	{
		std::cerr << "Invalid error_code \'" << errorCode << "\'." << std::endl;
		return ( false )
	}

	int nbErrorCode = atoi( errorCode.c_str() );
	if ( nbErrorCode < 400 || nbErrorCode > 599 )
	{
		std::cerr << "Invalid error_code \'" << errorCode << "\'." << std::endl;
		return ( false )
	}

	return ( true );
}

bool Config::isValidErrorCodePage( const std::string &errorCode, const std::string &page ) // On voit si les deux mis ensemble c'est bon
{
	return ( true );
}

void Config::ParseServerConfigErrorPages( const std::vector<std::string> &lineSplitted )
{
	if ( lineSplitted.size() <= 3 )
	{
		std::cerr << "error_pages: Not enough arguments." << std::endl;
		return ;
	}

	std::string page = lineSplitted( lineSplitted.size() - 1 );

	if ( !this->ParseServerConfigPage( page ) )
	{
		std::cerr << "Invalid error_page \'" << page << "\'." << std::endl;
		return ( false );
	}

	int code;
	for ( size_t i = 1; ( i < lineSplitted.size() - 1 ); ++i )
	{
		if ( this->isValidErrorCodePage( lineSplitted[i], page ) )
		{
			code = atoi( lineSplitted[i].c_str() );
			_error_pages[code] = page;
		}
	}
}


// Client Max Body Size
bool Config::isValidClientMaxBodySize( const std::string &client_max_body_size )
{
	// Cas trop long
	if ( client_max_body_size.size() > 10 )
	{
		std::cerr << "Invalid client_max_body_size \'" << client_max_body_size << '\'' << std::endl;
		return ( false );
	}

	char *end;
	long nb;
	nb = strtol( client_max_body_size, &end, 10 );

	// Cas negatif
	if ( nb <= 0 )
	{
		std::cerr << "Invalid client_max_body_size \'" << client_max_body_size << '\'' << std::endl;
		return ( false );
	}

	// Cas octet
	if ( *end == '\0' )
	{
		if ( nb > CLIENT_MAX_BODY_SIZE_O )
		{
			std::cerr << "Invalid client_max_body_size \'" << client_max_body_size << '\'' << std::endl;
			return ( false );
		}
		_client_max_body_size = ( ( int ) nb );
		return ( true );
	}

	// Cas kilo octet
	if ( ( *end == 'k' || *end == 'K' ) && ( *end + 1 ) == '\0' )
	{
		if ( nb > CLIENT_MAX_BODY_SIZE_KO )
		{
			std::cerr << "Invalid client_max_body_size \'" << client_max_body_size << '\'' << std::endl;
			return ( false );
		}
		_client_max_body_size = ( ( int ) nb ) * 1024;
		return ( true );
	}

	// Cas mega octet
	if ( ( *end == 'm' || *end == 'M' ) && ( *end + 1 ) == '\0' )
	{
		if ( nb > CLIENT_MAX_BODY_SIZE_MO )
		{
			std::cerr << "Invalid client_max_body_size \'" << client_max_body_size << '\'' << std::endl;
			return ( false );
		}
		_client_max_body_size = ( ( int ) nb ) * 1024 * 1024;
		return ( true );
	}

	// Cas autre
	std::cerr << "Invalid client_max_body_size \'" << client_max_body_size << '\'' << std::endl;
	return ( false );
}

void Config::ParseServerConfigClientMaxBodySize( const std::vector<std::string> &lineSplitted )
{
	if ( lineSplitted.size() > 2 )
	{
		std::cerr << "client_max_body_size: Too many arguments." << std::endl;
		return ;
	}

	if ( !_client_max_body_size.empty() )
	{
		std::cerr << "client_max_body_size is already set." << std::endl;
		return ;
	}

	this->isValidClientMaxBodySize( lineSplitted[1] ); // On assigne direct dans la fct.
}


// Parse Config
bool Config::ParseServerConfig( std::ifstream &configFile )
{
	std::string line;
	std::vector<std::string> lineSplitted;

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

		if ( lineSplitted[0] == "listen" ) // checker les doublons avant d'ajouter.
			this->ParseServerConfigListen( lineSplitted );

		else if ( lineSplitted[0] == "server_names" )
			this->ParseServerConfigName( lineSplitted );

		else if ( lineSplitted[0] == "error_pages" ) // checker les doublons avant d'ajouter et voir si c'est correct.
			this->ParseServerConfigErrorPages( lineSplitted );

		else if ( lineSplitted[0] == "client_max_body_size" )
			this->ParseServerConfigClientMaxBodySize( lineSplitted );

		// else if ( lineSplitted[0] == "Route" ) // A faire
		// 	this->ParseServerConfigRoute( lineSplitted );

		else
			std::cerr << "Invalid Server directive \'" << line << "\'." << std::endl;

	}

	return ( true );
}
