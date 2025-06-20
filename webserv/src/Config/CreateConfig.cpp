#include "Config.hpp"

std::vector<Config> CreateConfigs( std::ifstream &configFile )
{
	std::vector<Config> configs;
	std::string line;
	std::vector<std::string> lineSplitted;

	while ( std::getline( configFile, line ) )
	{
		lineSplitted = splitFromCharset( line, BLANK_CHARACTERS );

		if ( lineSplitted.size() == 0 )
			continue;

		if ( lineSplitted[0] == "Server" )
		{
			Config config;

			config.ParseServerConfig( configFile );
			// checker si c est bon bg
			configs.push_back( config );
		}

		else
		{
			std::cerr << "Cannot be parsed" << std::endl;
			throw parsingError();
			break;
		}
	}

	return ( configs );
}
