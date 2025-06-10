#include "Webserv.hpp"
#include <sstream>

Request::Request() :
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
	// std::cout << "REQUEST : \n" << req << "\n\n";

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
					return this->setError(HTTP_METHOD_NOT_ALLOWED, __LINE__);
			}
			else if (i == 1)
			{
				this->_route = token;
				// std::cout << "ROUTE: " << token << std::endl;
				if (!ValidateURI(this->_route))
				{
					std::cerr << "Invalid url\n";
					return this->setError(HTTP_BAD_REQUEST, __LINE__);
				}
			}
			else
			{
				if (token != "HTTP/1.1")
					return this->setError(HTTP_BAD_REQUEST, __LINE__);
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
				// std::string address;
				// address.append(server.getAddress());
				// address.push_back(':');
				// std::ostringstream oss;
				// oss << server.getPort();
				// address.append(oss.str().c_str());
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

std::string	Request::formatResponse(Server& server) {
	switch (this->_response_code)
	{
	case 500:
		return std::string(InternalERROR(*this));
	case 400:
		return std::string(BadRequest(*this));
	
	default:
		break;
	}

	std::ostringstream resp;

	std::fstream inputfile;
	Config& config = server.getConfig();
	std::vector<Route> routes = config.getRoutes();

	if (routes.size() == 0)
		return std::string(ERROR_404(*this));

	bool	valid = false;
	std::vector<Route>::iterator i = routes.begin();
	


	while (i != routes.end())
	{
		if (std::strncmp((*i).getName().c_str(), this->getRoute(), (*i).getName().size()) == 0)
		{
			valid = true;
			break ;
		}
		i++;
	}

	if (!valid)
		return std::string(ERROR_404(*this));


	std::string subroute = this->_route.substr((*i).getName().size(), this->_route.size());
	//std::cout << "subroute : " << subroute << std::endl;
	
	// std::cout << filepath << std::endl;
	std::string filepath = (*i).getRootDir() + subroute;
	if (std::string(this->getRoute()) == (*i).getName())
	{
		filepath = filepath + "/" + (*i).getIndexFile();
	}
	// else
	// 	filepath.append(this->getRoute());
	std::string	extention;

	for (std::string::iterator i = filepath.end(); i != filepath.begin(); i--)
	{
		if (*i == '.')
		{
			while (i != filepath.end())
			{
				extention.push_back(*i);
				i++;
			}
			break ;
		}
		else if (*i == '/')
			break ;
	}

	std::transform(extention.begin(), extention.end(), extention.begin(), ::toupper);
	if (extention.size() > 0)
	{
		switch (hashdjb2(extention.c_str()))
		{
		case CSS_DJB2:
			this->_response_headers.insert(std::make_pair("Content-Type:" ,"text/css; charset=utf-8"));
			break;
		case HTML_DJB2:
			this->_response_headers.insert(std::make_pair("Content-Type:" ,"text/html"));
			break;
		
		default:
			break;
		}
	}
	
	//Logger::debug(filepath.c_str());
	std::string str;
	if (access(filepath.c_str(), F_OK | R_OK) == 0) // file exist
	{
		if (is_directory(filepath.c_str()))
		{
			if (true)
			{
				std::vector<std::string> files;
				if (!getAllFilesFromDirectory(files, filepath.c_str()))
					return std::string(ERROR_404(*this));
				return formatDirectoryListing(files);
			}
			else 
				return std::string(ERROR_404(*this));
		}
		else
		{
			inputfile.open(filepath.c_str(), std::ios::in);
			//std::cout << "file exist\n";
			if (inputfile.fail())
			{
				//std::cout << "Unable to open " << "argv[1]" << std::endl;
				if (access(filepath.c_str(), F_OK | R_OK) == -1)
					return (inputfile.close(), std::string(ERROR_404(*this)));
			}
			else
				str = std::string((std::istreambuf_iterator<char>(inputfile)), std::istreambuf_iterator<char>());
		}
	}
	else
		return (std::string(ERROR_404(*this)));
	

    resp << "HTTP/1.1 " << this->_response_code << " OK\r\n";

	for (std::map<string, string>::iterator it = this->_response_headers.begin(); it != this->_response_headers.end(); it++)
	{
		resp << (*it).first SPACE << (*it).second << "\r\n";
	}
	


    resp << "Content-Length: " << str.length() << "\r\n"
         << "Connection: close\r\n"
    	 << "\r\n"
         << str;
	
	inputfile.close();

    return resp.str();
	(void)server;
}

void	Request::setError(int code, int line) {
	if (line > 0)
	{
		std::ostringstream oss;
		oss << "LINE: " <<  line;
		Logger::error(oss.str().c_str());
	}
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


bool		Request::getConnectionStatus() {
	return this->_keepalive;
}


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

