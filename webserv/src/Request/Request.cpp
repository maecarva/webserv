#include "Webserv.hpp"
#include <sstream>

Request::Request(Server& server, int fd) :
	_server(server),
	_response_code(),
	_method(),
	_route(),
	_protocol(),
	_host(),
	_keepalive(true),
	_headers(),
	_header_len(),
	_body(),
	_start(),
	_end(),
	_fd(fd),
	_query_string(),
	_response_headers()
{
	gettimeofday(&this->_start, NULL);
};

Request::~Request() {};

std::string	Request::CreateResponse() {
	Response	res(*this);

	std::string	response = res.BuildResponse();

	return response;
}
