#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include "Webserv.hpp"

#include "Route.hpp"


// Exemple request :
// GET / HTTP/1.1
// Host: google.fr
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
	Server&								_server;
	unsigned short						_response_code;
    t_METHOD							_method;
	Route								_corresponding_route;
    std::string							_route;
	std::string							_clean_route;
	std::string							_ressource_requested;
	std::string							_protocol;
	std::string							_host;
	bool								_keepalive;
    std::map<std::string, std::string>	_headers;
    int									_header_len;
	struct timeval 						_start;
	struct timeval 						_end;

	// response
    std::map<std::string, std::string>	_response_headers;
public:
    Request(Server& server);
    ~Request();

	void			parseRequest(const char *req, Server& server);
	void			logRequest(Server& server);
	std::string		formatResponse(Server& server);
	std::string		CreateResponse();
	bool			ValidateURI(std::string&	route);
	std::string		ExtractRessource(std::string& route);
	Route			FindCorrespondingRoute(std::string& requestedressource, bool *failed);


	// * Setters
	void	setError(int code, int line, const char *filename);
	void	setResponseCode(int code);

	// * Getters
	Server&			getServer() const;
	const char		*getMethod()	const;
	const char		*getRoute()	const;
	const char		*getCleanRoute()	const;
	const char		*getHost()	const;
	const char		*getProtocol()	const;
	unsigned short	getResponseCode()	const;
	bool			isKeepAlive()	const;
	Route			getCorrespondingRoute();
	std::string		getRequestedRessource();
};


class Route;
// utils functions
t_METHOD    getMethodByHash(std::string& token);
std::string	BuildFilePath(std::string rootdir, std::string ressource);
#endif