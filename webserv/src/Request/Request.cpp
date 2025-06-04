#include "Webserv.hpp"

Request::Request() :
    _method(),
    _route(),
    _headers(),
    _header_len()
{
	gettimeofday(&this->_start, NULL);
};

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

void	Request::parseRequest(const char *req, Server& server)
{
	// std::cout << "REQUEST : " << req << "\n\n";

	this->_response_code = HTTP_OK;
	std::string request(req);
	std::string token;
	std::istringstream iss(request);

	bool	firstline = true;
	std::string line;
	while (std::getline(iss, line)) {
		std::istringstream iss2(line);
		int	i = 0;
		while (firstline && iss2 >> token) { // PARSE first line of request
			if (i == 0)
			{
				t_METHOD	method = getMethodByHash(token);
				if (method == NONE)
					return this->setError(HTTP_METHOD_NOT_ALLOWED);
			}
			else if (i == 1)
				this->_route = token;  // ! ADD check for valid URI
			else
			{
				if (token != "HTTP/1.1")
					return this->setError(HTTP_BAD_REQUEST);
				this->_protocol = token;
			}
			i++;
		}
		firstline = false;

		while (iss2 >> token) {
			std::pair<std::string, std::string> pair = std::make_pair(token, "");
			std::string key = token;
			// ! check if key already exist
			while (iss2 >> token) {
				pair.second.append(token);
			}
			std::transform(key.begin(), key.end(), key.begin(), ::toupper);
			if (key == "HOST:")
			{
				std::string address;
				address.append(server.getAddress());
				address.push_back(':');
				std::ostringstream oss;
				oss << server.getPort();
				address.append(oss.str().c_str());
				if (address != pair.second)
				{
					Logger::error("Invalid host.");
					return this->setError(HTTP_BAD_REQUEST);
				}
			}
			else if (key == "CONNECTION:") {
				std::string second = pair.second;
				std::transform(second.begin(), second.end(), second.begin(), ::toupper);
				if (second == "KEEP-ALIVE")
					this->_keepalive = true;
				else if (second == "CLOSE")
					this->_keepalive = false;
				else
					return this->setError(HTTP_BAD_REQUEST);
			}
			this->_headers.insert(pair);
		}
	}

	// for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
	// {
	// 	std::cout << "PAIR : " << (*it).first << " | " << (*it).second << std::endl;
	// }


	(void)server;
};

Request::~Request() {};


void	Request::logRequest(Server& server) {
	long	seconds;
	long	micros;
	gettimeofday(&this->_end, NULL);


	seconds = this->_end.tv_sec - this->_start.tv_sec;
	micros = this->_end.tv_usec - this->_start.tv_usec;
	long time = seconds * 1000000 + micros;

	std::ostringstream oss;
	oss << this->getMethod() << " " << this->getRoute() << " " << this->getHost()
	<< " " << (this->_keepalive ? "keep-alive" : "close") SPACE;

	if (this->_response_code == HTTP_OK)
		oss << GRN << this->_response_code << reset;
	else
		oss << this->_response_code;
	oss  << " " << time << " us";


	Logger::info(oss.str().c_str());
	(void) server;

}

const char	*Request::formatResponse(Server& server) {
	std::ostringstream resp;

	std::fstream inputfile;
	inputfile.open("./static_pages/index.html", std::ios::in);
	if (inputfile.fail())
	{
		std::cout << "Unable to open " << "argv[1]" << std::endl;
		return InternalERROR();
	}

	std::string str((std::istreambuf_iterator<char>(inputfile)), std::istreambuf_iterator<char>());

	// std::cout << "File content" << str << std::endl << std::endl;
	resp << "HTTP/1.1 200 OK\r\nContent-Length: " << str.length() << "\r\n\r\n" << str;
	// std::cout << resp.str() << std::endl;
	return resp.str().c_str();
	(void)server;
}

void	Request::setError(int code) {
	Logger::error("setError");
	this->_response_code = code;
}

const char	*Request::getMethod() {

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

const char	*Request::getRoute() {
	return this->_route.c_str();
};

const char	*Request::getHost() {
	return this->_host.c_str();
};
