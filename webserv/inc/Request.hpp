#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include "Webserv.hpp"

// Exemple request :
// GET / HTTP/1.1
// Host: 127.0.0.1:3000
// User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate, br, zstd
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1
// Sec-Fetch-Dest: document
// Sec-Fetch-Mode: navigate
// Sec-Fetch-Site: none
// Sec-Fetch-User: ?1
// Priority: u=0, i

class Server;

typedef enum e_METHOD {
	GET,
	POST,
	PATCH,
	PUT,
	DELETE,
	HEAD,
	OPTIONS,
	TRACE,
	CONNECT,
	NONE
}   t_METHOD;

class Request
{
private:
	unsigned short						_response_code;

    t_METHOD							_method;
    std::string							_route;
	std::string							_protocol;
	std::string							_host;
	bool								_keepalive;
    std::map<std::string, std::string>	_headers;
    int									_header_len;
	struct timeval 						_start;
	struct timeval 						_end;
public:
    Request();
    ~Request();

	void	parseRequest(const char *req, Server& server);
	void		logRequest(Server& server);
	const char	*formatResponse(Server& server);


	// * Setters
	void	setError(int code);



	// * Getters
	const char	*getMethod();
	const char	*getRoute();
	const char	*getHost();
};


#endif