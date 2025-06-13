#include "Webserv.hpp"

bool    checkMatchingRoutes(std::vector<Route>& routes, Response& res, Route **route, bool *indexRequested) {
    std::string cleanroute = res.getRequest().getCleanRoute();
	std::cout << res.getRequest().getRoute() << std::endl;

    for (std::vector<Route>::iterator i = routes.begin(); i != routes.end(); i++)
    {
        if ((*i).getName() == cleanroute)
        {
			std::cout << (*i).getName() << std::endl;
            *route = &(*i);
			if (res.getRequest().getRoute() == (*i).getName())
				*indexRequested = true;
            return true;
        }
    }
    return false;
}

bool    checkMatchingMethod(Response& res, Route *route) {
    std::vector<std::string>	methods = route->getAllowedMethods();
    std::string					res_method = std::string(res.getRequest().getMethod());

    std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), res.getRequest().getMethod());
	return (it == methods.end() ? false : true);
}