#include "Webserv.hpp"

void	Request::parseRequest(const char *req, Server& server)
{
#ifdef DEBUG
	std::cout << "REQUEST : \n" << req << "\n\n";
#endif

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
    std::string cleaned = "";
    int        slash = 0;
    
    for (   std::string::iterator it = splitted[1].begin();
            it != splitted[1].end();
            it++
        )
    {
        if (*it == '/')
            slash++;
        if (slash >= 2)
            break ;
        cleaned.push_back(*it);
    }
    this->_clean_route = cleaned;

    // * Parse Protocol
    if (splitted[2] != "HTTP/1.1")
        return this->setError(HTTP_BAD_REQUEST, __LINE__, __FILENAME__);
    this->_protocol = splitted[2];

    // * Parse rest of request

    while (std::getline(iss, line)) {

		std::istringstream iss2(line);
        while (iss2 >> token) {
            std::pair<std::string, std::string> pair = std::make_pair(token, "");
            std::string key = token;

            while (iss2 >> token) {
                pair.second.append(token);
            }

            std::transform(key.begin(), key.end(), key.begin(), ::toupper);
            
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