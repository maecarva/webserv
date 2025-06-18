#include "Webserv.hpp"

bool	Request::ValidateURI(std::string&	route) {
	for (std::string::iterator it = route.begin(); it != route.end(); it++)
	{
		if (!(std::isalnum(*it)) && !(*it == '$' || *it == '-' || *it == '_' || *it == '.' || *it == '+' || *it == '!' || *it == '*' || *it == '\'' || *it == '(' || *it == ')' || *it == ',' || *it == '/')
			&& !(*it == ';' || *it == '/' || *it == '?' || *it == ':' || *it == '@' || *it == '=' || *it == '&'))
		{
			std::cout << "invalid char : " << *it << std::endl;
			return false;
		}
	}
	return true;
}

t_METHOD    getMethodByHash(std::string& token) {
    unsigned long hash = hashdjb2(token.c_str());

	switch (hash)
	{
	case GET_DJB2:
		return GET;
		break;
	case POST_DJB2:
		return POST;
		break;
	case PATCH_DJB2:
		return PATCH;
		break;
	case PUT_DJB2:
		return PUT;
		break;
	case DELETE_DJB2:
		return DELETE;
		break;
	case HEAD_DJB2:
		return HEAD;
		break;
	case OPTIONS_DJB2:
		return OPTIONS;
		break;
	case TRACE_DJB2:
		return TRACE;
		break;
	case CONNECT_DJB2:
		return CONNECT;
		break;
	
	default:
		return NONE;
		break;
	}
}


void	Request::logRequest(Server& server) {
	long	seconds;
	long	micros;
	gettimeofday(&this->_end, NULL);


	seconds = this->_end.tv_sec - this->_start.tv_sec;
	micros = this->_end.tv_usec - this->_start.tv_usec;
	long time = seconds * 1000000 + micros;

	std::ostringstream oss;
	oss << "Server:" SPACE << server.getConfig().getServerNames()[0] SPACE << this->getMethod() << " " << this->getRoute() << " " << this->getHost()
	<< " " << (this->_keepalive ? "keep-alive" : "close") SPACE;

	if (this->_response_code == HTTP_OK)
		oss << GRN << this->_response_code << reset;
	else
		oss << YEL << this->_response_code << reset;
	oss  << " " << time << " us";

	if (this->_response_code == HTTP_OK)
		Logger::info(oss.str().c_str());
	else
		Logger::warn(oss.str().c_str());

}
