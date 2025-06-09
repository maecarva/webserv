#include "Config.hpp"

// Allowed Methods
bool Config::isValidMethod( const std::string &method )
{
	if ( method.find_first_of( FORBIDDEN_NAME_CHARACTERS ) != std::string::npos ) // Nom invalide
	{
		std::cerr << "Invalid route method: \'" << method << "\'." << std::endl;
		return ( false );
	}

	for ( size_t i = 0; i < ( _routes[_routes.size() - 1].allowed_methods.size() ); ++i ) // Nom deja existant
	{
		if ( _routes[_routes.size() - 1].allowed_methods[i] == method )
		{
			std::cerr << "Route already has method: \'" << method << "\'." << std::endl;
			return ( false );
		}
	}

	return ( true );
}

void Config::ParseServerConfigRouteAllow( const std::vector<std::string> &lineSplitted )
{
	for ( size_t i = 1; i < lineSplitted.size(); ++i )
	{
		std::transform( lineSplitted[i].begin(), lineSplitted[i].end(), lineSplitted[i].begin(), ::toupper );
		if ( isValidMethod( lineSplitted[i] ) )
			_routes[_routes.size() - 1].allowed_methods.push_back( lineSplitted[i] );
	}
}



// Parse Route
void Config::ParseServerConfigRoute( std::ifstream &configFile, std::string &line, std::vector<std::string> &lineSplitted )
{
	Route route;

	if ( lineSplitted.size() != 2 )
	{
		// Skip tous les elements de la route
		return ;
	}

	route.route = lineSplitted[1]; // check si le nom est bon

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
			this->ParseServerConfigRouteAllow( lineSplitted );

		else if ( lineSplitted[0] == "index" )

		else if (  )
	}
	_routes.push_back( route );
}
