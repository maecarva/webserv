#include "Webserv.hpp"

Route	Request::FindCorrespondingRoute(std::string& requestedressource, bool *failed) {

	std::vector<Route> routes = this->getServer().getConfig().getRoutes();
	std::vector<Route>::iterator routeit = routes.begin();
	size_t	pos = std::string::npos;
	Route	route;


	while (routeit != routes.end())
	{
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
	while (it2 != route.end() && *it2 != '?')
	{
		ressource.push_back(*it2);
		it2++;
	}
	return ressource;
}

void	print_hex(const std::string& str) {
	for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
	{
		std::cout << *i << " = " << std::hex << (char)*i << std::endl;
	}
}

void	Request::parseRequest(std::string& req, Server& server)
{

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

	if (this->_route.find('?') != std::string::npos)
		this->ParseQueryArgs();

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
				std::string localhost_listen = "localhost:" + this->getServer().getConfig().getServerPort();

				if ( second == localhost_listen )
					goto SUCCESS_NAME;
				for ( size_t i = 0; i < this->getServer().getConfig().getServerNames().size(); ++i )
				{
					if ( this->getServer().getConfig().getServerNames()[i] == second )
					{
						goto SUCCESS_NAME;
					}
				}
				if (this->getServer().getConfig().getListen() != second)
					return this->setError(HTTP_BAD_REQUEST, __LINE__, __FILENAME__);
				}
SUCCESS_NAME:
            if (key == "CONNECTION:") {
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


    std::string body;
    size_t pos = req.find("\r\n\r\n");
    for (size_t i = pos + 4; i < req.size(); i++)
    {
        body.push_back(req[i]);
    }
    
    this->_body = body;

	if (this->_body.size() > (size_t)this->getServer().getConfig().getClientMaxBodySize())
		return setError(HTTP_REQUEST_ENTITY_TOO_LARGE, __LINE__, __FILENAME__);

    if ( request.find( "multipart/form-data; boundary=" ) != std::string::npos )
    {
        std::string limiter = this->_headers["CONTENT-TYPE:"];
        _chunkedLimiter = limiter.substr( 31 );
        std::map<std::string, std::string> result = this->extractDataFromChunkedBody( _body );
		
        // for ( std::map<std::string, std::string>::iterator it = result.begin(); it != result.end(); ++it )
        // {
		// 	std::cout << "'" << it->first << "''" << it->second << "'" << std::endl;
        // }
    }

	std::string content_type = this->_headers["CONTENT-TYPE:"];
    std::transform(content_type.begin(), content_type.end(), content_type.begin(), ::toupper);
	if (content_type == "APPLICATION/X-WWW-FORM-URLENCODED")
	{
		if (this->_method == GET)
		{
			size_t pos = this->_route.find('?');
			if (pos != std::string::npos)
				this->_query_string = this->_route.substr(pos + 1);
		}
		else if (this->_method == POST)
		{
			this->_query_string = this->_body;
		}
	}
	else {
		if (this->_method == GET && this->_route.find('?') != std::string::npos)
		{
			size_t pos = this->_route.find('?');
			if (pos != std::string::npos)
				this->_query_string = this->_route.substr(pos + 1);
		}
	}

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

void			Request::ParseQueryArgs() {
	std::string route = this->_route;
	size_t		pos = route.find('?');

	if (pos == std::string::npos)
		return ;
	
	std::string query = route.substr(pos + 1);
	if (query == "")
		return ;

	std::vector<std::string> splitted = splitFromCharset(query, "&");

	for (size_t i = 0; i < splitted.size(); i++)
	{
		std::string key;
		std::string value;
		pos = splitted[i].find('=');
		key = splitted[i].substr(0, pos);
		value = splitted[i].substr(pos + 1);
		this->_query_args.push_back(std::make_pair(key, value));
	}
	
}
