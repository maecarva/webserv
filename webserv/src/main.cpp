#include "Webserv.hpp"
#include "Config.hpp"

//bool	running = true;

// socket = bidirectionnal fd
// setsockopt = change socket behavior, here we use ipv4 address and allow reuse after

// bool	running = true;

// void	sig_handler(int sig) {
// 	running = false;
// 	Logger::info("Stopping servers.");
// 	(void)sig;
// }

// void	set_signals(void)
// {
// 	signal(SIGINT, sig_handler);
// }

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

int main( void )
{

	std::ifstream infile( "input.txt" );
	std::vector<Config> vec = CreateConfigs(infile);

	for (std::vector<Config>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		std::vector<Route> routes = (*it).getRoutes();
		(*it).PrintConfig();
		NEWLINE;
		for (std::vector<Route>::iterator it = routes.begin(); it != routes.end(); it++)
		{
			(*it).printRoute();
		}
		
		NEWLINE;
		NEWLINE;
	}

	return 0;
}
