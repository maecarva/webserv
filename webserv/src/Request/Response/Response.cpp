#include "Response.hpp"
#include "Webserv.hpp"


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
	std::string ressource =  this->getRequest().getRequestedRessource();
	std::string uploaddir = this->getRequest().getCorrespondingRoute().getUploadDir();
	size_t max_body_size = this->getRequest().getServer().getConfig().getClientMaxBodySize();
	std::string filepath = BuildFilePath( uploaddir, ressource );

	std::string body = this->getRequest().getBody();

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


// * check extention with _cgi OK
// * join requested ressource + dir OK
// * construct env QUERY_STRING
// * execve
// * format response

extern char **env;

std::string		Response::handleCGI() {
	Request& req = this->getRequest();
	std::string extension = "";
	std::string requested_ressource = req.getRequestedRessource();
	std::string	cgi_program;
	std::string scriptpath;
	std::string	query_string_env;

	for (std::string::iterator i = requested_ressource.end() - 1; i != requested_ressource.begin(); i--)
	{
		extension.push_back(*i);
		if (*i == '.')
			break ;
	}
	std::reverse(extension.begin(), extension.end());

	std::map<std::string, std::string> mapeuh = this->getRequest().getCorrespondingRoute().getValidsCGI();
	cgi_program = mapeuh[extension];

	if (cgi_program.empty())
		return ErrorResponse(HTTP_BAD_REQUEST);

	scriptpath = BuildFilePath(this->getRequest().getCorrespondingRoute().getRootDir(), requested_ressource);

	query_string_env = "QUERY_STRING=" + req.getQueryString();

	//std::cout << query_string_env << std::endl;


	size_t envsize = 0;
	while (env[envsize])
		envsize++;
	
	envsize++;

	char **new_env = new char*[envsize + 1];
	size_t i = 0;
	while (i < envsize - 1)
	{
		new_env[i] = strdup(env[i]);
		i++;
	}
	new_env[i] = strdup(query_string_env.c_str());
	new_env[i + 1] = NULL;


	std::vector<std::string> argv;
	argv.push_back(cgi_program);
	argv.push_back(scriptpath);

	std::string cgireturn = executeCgi(argv, new_env);

	i = 0;
	while (new_env[i])
		delete new_env[i++];
	delete[] new_env;

	std::ostringstream oss;
	oss << "HTTP/1.1 ";
	oss << HTTP_OK << " " << HttpMessageByCode(HTTP_OK) << "\r\n";
	oss << cgireturn;
	return oss.str();
}


// // Cgi
// bool Response::isCgi( void )
// {

// }

// void Response::handleCgi( void )
// {

// }



std::string	Response::BuildResponse()
{
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

	// format
	if (this->getRequest().getCorrespondingRoute().isCGI())
	{
		return this->handleCGI();
	}
	else if (this->getRequest().getCorrespondingRoute().isRedirect())
		return this->formatRedirectResponse();
	else if (this->getRequest().getCorrespondingRoute().getUploads())
		return this->handleUploadResponse();
	else if ( indexRequested && route.getAutoIndex() )
	{
		if ( !this->ReadFile( BuildFilePath( route.getRootDir(), route.getIndexFile() ).c_str(), responseFileContent, mime_type ) )
			return ( ErrorResponse( 404 ) );

		return this->formatResponse( responseFileContent, HTTP_OK, mime_type );
	}
	// else if ( this->getRequest().isCgi() )
	// {
	// 	return ( this->handleCgi() );
	// }

	else
	{
		if ( !this->ReadFile( BuildFilePath( route.getRootDir(), this->getRequest().getRequestedRessource() ).c_str(), responseFileContent, mime_type ) )
			return ( ErrorResponse( 404 ) );
		
		return this->formatResponse( responseFileContent, HTTP_OK, mime_type );
	}
	
	return ErrorResponse( 500 );
}
