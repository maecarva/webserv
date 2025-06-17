#include "Webserv.hpp"

Route	Request::FindCorrespondingRoute(std::string& requestedressource, bool *failed) {
	
	//std::cout << "Requested: " << requestedressource << "\n";

	std::vector<Route> routes = this->getServer().getConfig().getRoutes();
	std::vector<Route>::iterator routeit = routes.begin();
	size_t	pos = std::string::npos;
	Route	route;
	while (routeit != routes.end())
	{
		//std::cout << (*routeit).getName() << std::endl;
		pos = requestedressource.find((*routeit).getName());
		if (pos != std::string::npos && pos == 0) {
			if (*failed)
			{
				if (route.getName().size() < (*routeit).getName().size())
				route = *routeit;
			}
			else {
				*failed = true;
				route = *routeit;
			}
		}
		routeit++;
	}
	return route;
}


std::string		Request::ExtractRessource(std::string& route) {
	std::string::iterator it = this->_corresponding_route.getName().begin();
	std::string::iterator it2 = route.begin();
	while (it != this->_corresponding_route.getName().end() 
			&& it2 != route.end() && *it == *it2)
	{
		it++;
		it2++;
	}
	std::string ressource = "";
	while (it2 != route.end())
	{
		ressource.push_back(*it2);
		it2++;
	}
	return ressource;
}


void	Request::parseRequest(std::string& req, Server& server)
{
// #ifdef DEBUG
	//std::cout << "request size: " << req << std::endl;
	std::cout << "REQUEST : \n";
	PRINTCLN(RED, req.c_str());
// #endif

    this->_response_code = HTTP_OK;
    std::string         token;
    std::string         line;
    std::string         request(req);
    std::istringstream  iss(request);

    // * Parsing of first line in request : GET / HTTP/1.1\r\n
    std::getline(iss, line);
    std::vector<std::string> splitted = splitFromCharset(line, " \r\n\v\f\t");
    if (splitted.size() != 3)
        return this->setError(HTTP_BAD_REQUEST, __LINE__, __FILENAME__);

    // * Parse method
    t_METHOD	method = getMethodByHash(splitted[0]);
    if (method == NONE)
        return this->setError(HTTP_METHOD_NOT_ALLOWED, __LINE__, __FILENAME__);
    this->_method = method;

    // * Parse Route && cleaned route
    this->_route = splitted[1];
    if (!ValidateURI(this->_route))
    {
        Logger::warn("Invalid url");
        return this->setError(HTTP_BAD_REQUEST, __LINE__, __FILENAME__);
    }

	bool	failed = false;
	Route matchedroute = this->FindCorrespondingRoute(splitted[1], &failed);
	if (!failed)
		return this->setError(HTTP_NOT_FOUND, __LINE__, __FILENAME__);

	this->_corresponding_route = matchedroute;
	this->_clean_route = matchedroute.getName();
	this->_ressource_requested = this->ExtractRessource(splitted[1]);

    // * Parse Protocol
    if (splitted[2] != "HTTP/1.1")
        return this->setError(HTTP_BAD_REQUEST, __LINE__, __FILENAME__);
    this->_protocol = splitted[2];

    // * Parse rest of request

    while (std::getline(iss, line)) {

		std::istringstream iss2(line);
        while (iss2 >> token) {
            std::string key = token;
            
            std::transform(key.begin(), key.end(), key.begin(), ::toupper);
            std::pair<std::string, std::string> pair = std::make_pair(key, "");

            while (iss2 >> token) {
                pair.second.append(token);
            }

            
            if (key == "HOST:") { 
                std::string second = pair.second;
                this->_host = second;
            }
            else if (key == "CONNECTION:") {
                std::string second = pair.second;
                std::transform(second.begin(), second.end(), second.begin(), ::toupper);
                if (second == "KEEP-ALIVE")
                    this->_keepalive = true;
                else if (second == "CLOSE")
                    this->_keepalive = false;
                else
                    this->_keepalive = false;
            }

            this->_headers.insert(pair);
        }
    }


    // std::vector<unsigned char> body;

    // size_t pos = request.find("\r\n\r\n");

	// if (pos != std::string::npos)
	// {
	// 	int tmppos = pos + 4;
	// 	while (static_cast<size_t>(tmppos) <= request.size())
	// 	{
	// 		body.push_back(request[tmppos]);
	// 		if (tmppos - (long)pos > server.getConfig().getClientMaxBodySize())
	// 		{
	// 			PRINTCLN(RED, "max body size reached");
	// 			return this->setError(HTTP_BAD_REQUEST, __LINE__, __FILENAME__);
	// 		}
	// 		tmppos++;
	// 	}
	// }

    // std::string headerinfo = this->_headers["CONTENT-TYPE:"];
    // std::transform(headerinfo.begin(), headerinfo.end(), headerinfo.begin(), ::toupper);
    // if ( !headerinfo.empty() ) // si y'a un content type.
    // {
    //     if (headerinfo == "APPLICATION/OCTET-STREAM" || headerinfo.find("MULTIPART/FORM-DATA;") != std::string::npos) {
    //         ssize_t	count = -1;
    //         while (true) {
    //         char	buf[BUFSIZ * 4];
	// 		count = recv(this->_fd, buf, sizeof(buf), 0);
	// 			if (count <= 0)
    //                 break ;
    //             PRINTCLN(GRN, "recv");
    //             for (ssize_t i = 0; i < count; i++)
    //             {
    //                 body.push_back(buf[i]);
    //             }
    //         }
    //     }
    // }

    // this->_body = body;

    // for (size_t i = 0; i < pos; i++)
    // {
    //     std::cout << req[i];
    // }
    // std::cout << "\r\n\r\n";
    // for (size_t i = 0; i < this->_body.size(); i++)
    // {
    //     std::cout << this->_body[i];
    // }
    // std::cout << "\r\n\r\n";



#ifdef DEBUG
    PRINTCLN(GRN, "PARSED = ");
    std::cout << "getResponseCode: " << this->getResponseCode();
    NEWLINE;
    std::cout << "getMethod: " << this->getMethod();
    NEWLINE;
    std::cout << "getRoute: " << this->getRoute();
    NEWLINE;
    std::cout << "getCleanRoute: " << this->getCleanRoute();
    NEWLINE;
    std::cout << "getProtocol: " << this->getProtocol();
    NEWLINE;
    std::cout << "getHost: " << this->getHost();
    NEWLINE;
    std::cout << "isKeepAlive: " << this->isKeepAlive();
    NEWLINE;
    std::cout << "Headers = \n";
    for (std::map<std::string, std::string>::iterator i = this->_headers.begin(); i != this->_headers.end(); i++)
    {
        std::cout << (*i).first << "|" << (*i).second << std::endl;
    }
#endif
    (void)server;
};
