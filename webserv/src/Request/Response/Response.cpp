#include "Response.hpp"
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


Response::Response(Request& req) :
	_req(req),
	_default_response()
{};

Response::~Response() {};


// TODO:
// * - Check if route_clean match any routes in config /site
// * - Check if Method is valid for route OK
// - Check if route == index route => return index file if autoindex
// - Check if file requested exist ? file : 404
// - Set MIME type
// - return Formated HTTP Response

std::string	Response::BuildResponse() {
#ifdef DEBUG
	Logger::debug("Response::BuildResponse");
#endif

	if (this->CheckErrors())
		return this->_default_response.c_str();

	// Server&				server 				= this->getRequest().getServer();
	// Config&				config 				= server.getConfig();
	Route				route				= this->getRequest().getCorrespondingRoute();
	bool				indexRequested		= false;
	std::string			responseFileContent = "";


	if (this->_req.getRequestedRessource() == "/" || this->_req.getRequestedRessource() == "")
		indexRequested = true;

	if (!checkMatchingMethod(*this, &route)) {
		return (ErrorResponse(HTTP_BAD_REQUEST));
	}

	if (indexRequested && route.getAutoIndex()) {
		if (!ReadFile(BuildFilePath(route.getRootDir(), route.getIndexFile()).c_str(), responseFileContent))
			return (ErrorResponse(404));
		
		std::ostringstream oss;

		oss << "HTTP/1.1 200 OK\r\nContent-Length: " << responseFileContent.size() << "\r\nContent-type: text/html\r\n\r\n";
		oss << responseFileContent;

		return oss.str();
	} else {
		if (!ReadFile(BuildFilePath(route.getRootDir(), this->getRequest().getRequestedRessource()).c_str(), responseFileContent))
			return (ErrorResponse(404));
		
		std::ostringstream oss;

		oss << "HTTP/1.1 200 OK\r\nContent-Length: " << responseFileContent.size() << "\r\nContent-type: text/css\r\n\r\n";
		oss << responseFileContent;

		return oss.str();

	}
	
	//std::string	file_requested = extractFileFromRoute(route->getName(), this->getRequest().getCleanRoute())
	// Make a function which takes a path to a file and build proper HTTP response;
	
}

