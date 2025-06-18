#include "Webserv.hpp"

void	Request::setError(int code, int line, const char *filename) {
	if (line > 0)
	{
		std::ostringstream oss;
		oss << filename << ":" << line;
		Logger::error(oss.str().c_str());
	}
	this->_response_code = code;
}

void	Request::setResponseCode(int code) {
	this->_response_code = code;
}

const char	*Request::getMethod() const {

	switch (this->_method)
	{
	case GET:
		return "GET";
		break;
	case POST:
		return "POST";
		break;
	case PATCH:
		return "PATCH";
		break;
	case PUT:
		return "PUT";
		break;
	case DELETE:
		return "DELETE";
		break;
	case HEAD:
		return "HEAD";
		break;
	case OPTIONS:
		return "OPTIONS";
		break;
	case TRACE:
		return "TRACE";
		break;
	case CONNECT:
		return "CONNECT";
		break;
	
	default:
		return "";
		break;
	}

};

const char	*Request::getRoute() const {
	return this->_route.c_str();
};

const char	*Request::getHost() const {
	return this->_host.c_str();
};

bool		Request::isKeepAlive() const {
	return this->_keepalive;
}

const char		*Request::getCleanRoute() const {
    return this->_clean_route.c_str();
};

const char		*Request::getProtocol() const {
    return this->_protocol.c_str();
};

unsigned short	Request::getResponseCode() const {
    return this->_response_code;
};

Server&			Request::getServer() const {
	return this->_server;
}

Route			Request::getCorrespondingRoute() {
	return this->_corresponding_route;
}

std::string		Request::getRequestedRessource() {
	return this->_ressource_requested;
}

std::string		Request::getBody() {
	return this->_body;
}

void			Request::pushBody(std::vector<unsigned char>& newpart) {
	this->_body.insert(this->_body.end(), newpart.begin(), newpart.end());
}
