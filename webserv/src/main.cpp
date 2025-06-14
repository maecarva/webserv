#include "Webserv.hpp"
#include "Config.hpp"

bool	running = true;

void	sig_handler(int sig) {
	running = false;
	Logger::info("Stopping servers.");
	(void)sig;
}

void	set_signals(void)
{
	signal(SIGINT, sig_handler);
}

int main( void )
{

	std::ifstream infile( "miniconfig.txt" );
	std::vector<Config> vec = CreateConfigs(infile);

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
