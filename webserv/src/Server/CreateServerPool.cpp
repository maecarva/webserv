#include "Webserv.hpp"

std::vector<Server>	CreateServerPool(std::vector<Config>& configs)
{
	// create servers

	std::vector<Server>	servers;

	for (	std::vector<Config>::iterator it = configs.begin();
			it != configs.end();
			it++
		)
	{
		servers.push_back(Server(*it));
	}

	return servers;
}