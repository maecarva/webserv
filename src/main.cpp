#include "Webserv.hpp"
#include "Config.hpp"

bool	running = true;

char	**env;

void	sig_handler(int sig) {
	running = false;
	Logger::info("Stopping servers.");
	(void)sig;
}

void	set_signals(void)
{
	signal(SIGINT, sig_handler);
}

int main( int ac, char **av , char **envp)
{
	if (ac != 2)
	{
		std::cerr << "Usage: ./webserv <configfile>" << std::endl;
		return -1;
	}

	env = envp;

	std::ifstream infile( av[1] );
	std::vector<Config> vec;

	try {
		vec = CreateConfigs(infile);
		if ( vec.size() == 0)
		{
			std::cerr << "No config" << std::endl;
			throw parsingError();
		}
		for ( size_t i = 0; i < vec.size(); ++i )
		{
			if ( !vec[i].checkConfig() )
				throw parsingError();
		}

	} catch ( ... )
	{
		return ( -1 );
	}

	Config::PrintConfig(vec);

	
	std::vector<Server> servers;
	set_signals();

	try
	{
		servers = CreateServerPool(vec);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		goto _CLEAN;
	}
	
	while (running) {
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			(*it).handler();
		}
	}

_CLEAN:
	// clear servers
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		Logger::info("Closing server.");
	}
	servers.clear();

	return 0;
}
