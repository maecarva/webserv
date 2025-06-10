#include "Webserv.hpp"
#include "Config.hpp"



// int main(int argc, char const *argv[])
// {
// 	std::vector<Server*> servers;

// 	set_signals();
// 	tryConfig::
// 	{
// 		Server *server1 = new Server(3000, "127.0.0.1", "server1");
// 		servers.push_back(server1);

// 		Server *server2 = new Server(3001, "127.0.0.1", "server2");
// 		servers.push_back(server2);
// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 	}


// 	while (running) {
// 		for (std::vector<Server*>::iterator it = servers.begin(); it != servers.end(); ++it)
// 		{
// 			(*it)->handler();
// 		}
// 	}

// 	// clear servers
// 	for (std::vector<Server*>::iterator it = servers.begin(); it != servers.end(); ++it)
// 	{
// 		std::ostringstream oss;
// 		oss << "Closing server: '" << (*it)->getLabel();
// 		Logger::info(oss.str().c_str());
// 		delete *it;
// 	}
// 	servers.clear();
	
// 	return (0 * argc * argv[0][0]);
// }


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

	std::ifstream infile( "input.txt" );
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
