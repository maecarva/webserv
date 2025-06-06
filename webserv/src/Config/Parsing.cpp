#include "Config.hpp"


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
		std::cerr << "getaddrinfo error: " << gai_strerror(err) << std::endl;
		return ( false );
	}
	freeaddrinfo( res );

	return ( true );
}

void Config::ParseServerConfigListen( std::vector<std::string> lineSplitted )
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
		std::cerr << "Invalid server name: \'" << lineSplitted[i] << '\'' << std::endl;
		return ( false );
	}

	for ( size_t i = 0; i < _name.size(); ++i ) // Nom deja existant
	{
		if ( _name[i] == name )
		{
			std::cerr << "Server already has name: \'" << lineSplitted[i] << '\'' << "" << std::endl;
			return ( false );
		}
	}

	return ( true );
}

void Config::ParseServerConfigName( std::vector<std::string> lineSplitted )
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
		std::cerr << "Invalid error_code \'" << errorCode << '\'' << std::endl;
		return ( false )
	}

	int nbErrorCode = atoi( errorCode.c_str() );
	if ( nbErrorCode < 400 || nbErrorCode > 599 )
	{
		std::cerr << "Invalid error_code \'" << errorCode << '\'' << std::endl;
		return ( false )
	}

	return ( true );
}

void Config::ParseServerConfigErrorPages( std::vector<std::string> lineSplitted )
{
	if ( lineSplitted.size() <= 3 )
	{
		std::cerr << "error_pages: Not enough arguments" << std::endl;
		return ;
	}

	for ( size_t i = 1; ( i < lineSplitted(size) - 1 ); ++i )
	{
		if (  )
	}
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

		else if ( lineSplitted[0] == "listen" )
			ParseServerConfigListen( lineSplitted );

		else if ( lineSplitted[0] == "server_names" )
			ParseServerConfigName( lineSplitted );

		else if ( lineSplitted[0] == "error_pages" )
			ParseServerConfigErrorPages( lineSplitted );





	}

	return ( true );
}
