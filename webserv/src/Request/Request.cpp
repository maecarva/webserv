#include "Webserv.hpp"
#include <sstream>

Request::Request(Server& server) :
	_server(server),
	_response_code(),
	_method(),
	_route(),
	_protocol(),
	_host(),
	_keepalive(),
	_headers(),
	_header_len(),
	_start(),
	_end(),
	_response_headers()
{
	gettimeofday(&this->_start, NULL);
};

Request::~Request() {};


std::string	Request::CreateResponse() {
	Response	res(*this);

	return std::string(res.BuildResponse());
}



