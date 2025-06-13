#include "Webserv.hpp"


std::string	Request::formatResponse(Server& server) {

	// ? check error on request parsing
	switch (this->_response_code)
	{
	case 500:
		return std::string(InternalERROR());
	case 400:
		return std::string(BadRequest());
	
	default:
		break;
	}

	std::ostringstream resp;

	std::fstream inputfile;
	Config& config = server.getConfig();
	std::vector<Route> routes = config.getRoutes();

	if (routes.size() == 0)
		return std::string(ERROR_404());

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
		return std::string(ERROR_404());


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
					return std::string(ERROR_404());
				return formatDirectoryListing(files);
			}
			else 
				return std::string(ERROR_404());
		}
		else
		{
			inputfile.open(filepath.c_str(), std::ios::in);
			//std::cout << "file exist\n";
			if (inputfile.fail())
			{
				//std::cout << "Unable to open " << "argv[1]" << std::endl;
				if (access(filepath.c_str(), F_OK | R_OK) == -1)
					return (inputfile.close(), std::string(ERROR_404()));
			}
			else
				str = std::string((std::istreambuf_iterator<char>(inputfile)), std::istreambuf_iterator<char>());
		}
	}
	else
		return (std::string(ERROR_404()));
	

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


Response::Response(const Request& req) :
	_req(req),
	_default_response()
{};

Response::~Response() {};

const char   *Response::BuildResponse() {
	if (this->CheckErrors())
		return this->_default_response.c_str();
}