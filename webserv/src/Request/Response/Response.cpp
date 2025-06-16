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

std::string	Response::getMIMEtype() {
	std::string ressource = this->getRequest().getRequestedRessource();
	std::string ext = "";
	if (ressource.find('.') == std::string::npos)
		return "Content-type: text/plain";
	for (std::string::iterator rit = ressource.end() - 1; rit != ressource.begin(); rit--)
	{
		ext.push_back(*rit);
		if (*rit == '.')
			break ;
	}

	std::reverse(ext.begin(), ext.end());
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
	unsigned long hashed = hashdjb2(ext.c_str());
	switch (hashed)
	{
	case TXT_DJB2:
		return "Content-type: text/plain";
		break;
	case JSON_DJB2:
		return "Content-type: application/json";
		break;
	case HTML_DJB2:
		return "Content-type: text/html";
		break;
	case CSS_DJB2:
		return "Content-type: text/css";
		break;
	case JS_DJB2:
		return "Content-type: text/javascript";
		break;
	case PNG_DJB2:
		return "Content-type: image/png";
		break;

	default:
		return "Content-type: image/png";      // a enlever et a gerer proprement stp
		return "Content-type: text/plain";
		break;
	}
}

std::string		Response::formatResponse(std::string& responseFileContent, int responseCode, std::string& mimetype) {
	std::ostringstream oss;

	oss << "HTTP/1.1 ";
	oss << responseCode << " " << HttpMessageByCode(responseCode);
	oss << "\r\nContent-Length: " << responseFileContent.size() << "\r\n";
	if (mimetype.size() > 0)
		oss << mimetype << "\r\n\r\n";
	else
		oss << this->getMIMEtype() << "\r\n\r\n";
	oss << responseFileContent;

	return oss.str();
}

std::string		Response::formatRedirectResponse() {
	std::ostringstream oss;

	oss << "HTTP/1.1 ";
	oss << HTTP_FOUND << " " << HttpMessageByCode(HTTP_FOUND) << "\r\n";
	oss << "Location: " << this->getRequest().getCorrespondingRoute().getReturn() << "\r\n\r\n";

	return oss.str();
}

std::string		Response::handleUploadResponse() {
	std::string ressource =  this->getRequest().getRequestedRessource();                  // c est quoi corresponding route ??
	std::string uploaddir = this->getRequest().getCorrespondingRoute().getUploadDir();
	size_t max_body_size = this->getRequest().getServer().getConfig().getClientMaxBodySize();
	std::string filepath = BuildFilePath( uploaddir, ressource );

	const std::vector<unsigned char>& body = this->getRequest().getBody();

	std::cout << body.size() << std::endl;
	std::cout << max_body_size << std::endl;

	if ( max_body_size >= body.size() )          // verifier si le fichier existe
	{
		std::ofstream ofs( filepath.c_str(), std::ios::binary );
		if ( !ofs.is_open() )
		{
			std::cerr << "Erreur: impossible d'écrire dans " << filepath << std::endl;
			return ( ErrorResponse(500) );
		}
		
		ofs.write( reinterpret_cast<const char*>( body.data() ), body.size() );
		ofs.close();
	}
	else
	{
		std::cout << "File to big" << std::endl;
		return ( ErrorResponse(500) );
	}


	std::ostringstream oss;

	oss << "HTTP/1.1 ";
	oss << HTTP_OK << " " << HttpMessageByCode(HTTP_OK) << "\r\n";
	oss << "Content-Type: text/plain\r\n";
	oss << "Content-Length: 13\r\n\r\n";
	oss << "File created.";

	return oss.str();
};

// TODO :
// * - Check if route_clean match any routes in config /site
// * - Check if Method is valid for route
// * - Check if route == index route => return index file if autoindex
// * - Check if file requested exist ? file : 404
// * - Set MIME type
// * - return Formated HTTP Response
// - Change body to be a byte vector and not a string to prevent 0x00 anywhere in the file.

std::string	Response::BuildResponse() {
#ifdef DEBUG
	Logger::debug("Response::BuildResponse");
#endif

	if (this->getRequest().getResponseCode() != HTTP_OK)
		return ErrorResponse(this->getRequest().getResponseCode());

	Route				route				= this->getRequest().getCorrespondingRoute();
	bool				indexRequested		= false;
	std::string			responseFileContent = "";
	std::string			mime_type			= "";

	if (this->_req.getRequestedRessource() == "/" || this->_req.getRequestedRessource() == "")
		indexRequested = true;

	if (!checkMatchingMethod(*this, &route)) {
		return (ErrorResponse(HTTP_BAD_REQUEST));
	}

	for ( int i = 0; i < this->getRequest().getBody()[i]; ++i )
		std::cout << this->getRequest().getBody()[i] << std::endl;

	// format
	if (this->getRequest().getCorrespondingRoute().isRedirect())
		return this->formatRedirectResponse();
	else if (this->getRequest().getCorrespondingRoute().getUploads())
		return this->handleUploadResponse();
	else if (indexRequested && route.getAutoIndex()) {
		if (!this->ReadFile(BuildFilePath(route.getRootDir(), route.getIndexFile()).c_str(), responseFileContent, mime_type))
			return (ErrorResponse(404));

		return this->formatResponse(responseFileContent, HTTP_OK, mime_type);
	} else {
		if (!this->ReadFile(BuildFilePath(route.getRootDir(), this->getRequest().getRequestedRessource()).c_str(), responseFileContent, mime_type))
			return (ErrorResponse(404));
		
		return this->formatResponse(responseFileContent, HTTP_OK, mime_type);
	}
	
	return ErrorResponse(500);
}
