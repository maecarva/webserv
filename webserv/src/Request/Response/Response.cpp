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
	
	if (strcmp(this->getRequest().getMethod(), "HEAD") != 0)
		oss << responseFileContent;

	return oss.str();
}

std::string		Response::formatRedirectResponse() {
	std::ostringstream oss;

	oss << "HTTP/1.1 ";
	oss << HTTP_FOUND << " " << HttpMessageByCode(HTTP_FOUND) << "\r\n";
	oss << "Content-Length: 0\r\n";
	oss << "Location: " << this->getRequest().getCorrespondingRoute().getReturn() << "\r\n\r\n";

	return oss.str();
}

std::string		Response::handleUploadResponse() {
	std::string ressource =  this->getRequest().getRequestedRessource();
	std::string uploaddir = this->getRequest().getCorrespondingRoute().getUploadDir();
	size_t max_body_size = this->getRequest().getServer().getConfig().getClientMaxBodySize();
	std::string filepath = BuildFilePath( uploaddir, ressource );

	std::string body = this->getRequest().getBody();
	std::string mime_type;
	int error_code = HTTP_OK;
	std::string responseFileContent;

	if (this->getRequest().getMethod() == std::string("GET")) {
		if ( !this->ReadFile( BuildFilePath(this->getRequest().getCorrespondingRoute().getUploadDir() , this->getRequest().getRequestedRessource() ).c_str(), responseFileContent, mime_type , &error_code) )
				return ( ErrorResponse( error_code ) );
		return this->formatResponse( responseFileContent, HTTP_OK, mime_type );
	}
	else if ( max_body_size >= body.size() )          // verifier si le fichier existe
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

	if (req.getQueryString().empty())
		return ErrorResponse(HTTP_BAD_REQUEST);
	query_string_env = "QUERY_STRING=" + req.getQueryString();

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

	if (cgireturn.empty())
	{
		if (access(argv[1].c_str(), F_OK) == 0 && access(argv[1].c_str(), F_OK | R_OK) == -1)
			return ErrorResponse(500);
		else
			return ErrorResponse(404);
	}

	std::ostringstream oss;
	oss << "HTTP/1.1 ";
	oss << HTTP_OK << " " << HttpMessageByCode(HTTP_OK) << "\r\n";
	oss << cgireturn;

	return oss.str();
}

std::string	Response::BuildResponse()
{
#ifdef DEBUG
	Logger::debug("Response::BuildResponse");
#endif

	if (this->getRequest().getResponseCode() != HTTP_OK)
		return ErrorResponse(this->getRequest().getResponseCode());

	Route				route				= this->getRequest().getCorrespondingRoute();
	std::string			responseFileContent = "";
	std::string			mime_type			= "";
	int					error_code			= HTTP_OK;

	if (!checkMatchingMethod(*this, &route)) {
		return (ErrorResponse(HTTP_METHOD_NOT_ALLOWED));
	}

	// format
	PRINTCLN(RED, "LA");
	if ( !this->getRequest().getCorrespondingRoute().getProtection().empty()   )
	{
		std::ostringstream oss;
		std::string cookie;
		std::map<std::string, unsigned int> &tokenmap = this->getRequest().getServer().getTokenMap();

		std::string cookie2;

		if (this->getRequest().getBody().find("username") != std::string::npos )
		{
			size_t i = this->getRequest().getBody().find("username");
			size_t j = this->getRequest().getBody().find("\r\n", i);
			cookie = this->getRequest().getBody().substr( i, j - i );
			if ( tokenmap.count(cookie) == 0 )
			{
				tokenmap[cookie] = 0;
			}

			oss << "HTTP/1.1 ";
			oss << HTTP_FOUND << " " << HttpMessageByCode(HTTP_FOUND) << "\r\n";
			oss << "Set-Cookie: token=" << cookie << "; Path=/; Max-Age=3600\r\n";
			oss << "Content-Length: 0\r\n";
			oss << "Location: " << this->getRequest().getCorrespondingRoute().getProtection() << "\r\n\r\n";

			return oss.str();
		}

		std::string headercookie = this->getRequest().getHeaders()["COOKIE:"];
		if (headercookie.size() > 6)
			cookie2 = headercookie.substr(6);

		if (tokenmap.count(cookie2) > 0){
			oss << "HTTP/1.1 ";
			oss << HTTP_FOUND << " " << HttpMessageByCode(HTTP_FOUND) << "\r\n";
			oss << "Content-Length: 0\r\n";
			oss << "Location: " << this->getRequest().getCorrespondingRoute().getProtection() << "\r\n\r\n";
			return oss.str();
		}
		if (!this->ReadFile( BuildFilePath(route.getRootDir(), route.getIndexFile()).c_str(), responseFileContent, mime_type , &error_code))
			return ( ErrorResponse( error_code ) );
		return this->formatResponse( responseFileContent, HTTP_OK, mime_type );
	}
	PRINTCLN(RED, "LA");
	// PRINTCLN(RED, this->getRequest().getQueryString());
	if ( this->getRequest().getCorrespondingRoute().getGuard() )
	{
		std::string cookie2;
		std::string cookieheader = this->getRequest().getHeaders()["COOKIE:"];
		if (cookieheader.size() > 6)
			cookie2 = cookieheader.substr(6);
		std::cout << "cokieheader: " << cookieheader << " cookie: " << cookie2 << std::endl;
		std::map< std::string, unsigned int >& tokenmap = this->getRequest().getServer().getTokenMap();

		if (tokenmap.count(cookie2) == 0 || cookieheader == "" || cookie2 == "") {
			std::vector<Route> routes = this->getRequest().getServer().getConfig().getRoutes();
			std::string returnlocation = this->getRequest().getCorrespondingRoute().getReturn();

			PRINTCLN(RED, "if");
			if (returnlocation.empty())
				return (ErrorResponse(HTTP_BAD_REQUEST));
			std::ostringstream oss;
			PRINTCLN(RED, "if2");

			oss << "HTTP/1.1 ";
			oss << HTTP_FOUND << " " << HttpMessageByCode(HTTP_FOUND) << "\r\n";
			oss << "Content-Length: 0\r\n";
			oss << "Location: " << this->getRequest().getCorrespondingRoute().getReturn() << "\r\n\r\n";

			std::cout << oss.str() << std::endl;
			return oss.str();
		} else {
			tokenmap[cookie2] += 1;
			PRINTCLN(RED, "else");
			//int integer = tokenmap[cookie];
			if (!this->ReadFile( route.getGuardPage().c_str(), responseFileContent, mime_type , &error_code))
				return ( ErrorResponse( error_code ) );
			return this->formatResponse( responseFileContent, HTTP_OK, mime_type );
		}
		
	}
	else if (this->getRequest().getCorrespondingRoute().isCGI())
	{
		return this->handleCGI();
	}
	else if (this->getRequest().getCorrespondingRoute().isRedirect())
		return (  this->formatRedirectResponse() );
	else if (this->getRequest().getCorrespondingRoute().getUploads())
		return  (  this->handleUploadResponse() );
	else {
		std::string filepath = BuildFilePath(route.getRootDir() , this->getRequest().getRequestedRessource());

		if (is_directory(filepath.c_str()) && route.indexFileIsSet())
		{
			filepath = BuildFilePath(filepath, route.getIndexFile());
			if ( !this->ReadFile( filepath.c_str(), responseFileContent, mime_type , &error_code) )
			{
				filepath = BuildFilePath(route.getRootDir() , this->getRequest().getRequestedRessource());
				if (!this->ReadFile( filepath.c_str(), responseFileContent, mime_type , &error_code))
					return ( ErrorResponse( error_code ) );
			}
			return this->formatResponse( responseFileContent, HTTP_OK, mime_type );
		}
		else if (is_directory(filepath.c_str()) && !route.indexFileIsSet() && route.getAutoIndex())
		{
			if ( !this->ReadFile( BuildFilePath(route.getRootDir() , this->getRequest().getRequestedRessource() ).c_str(), responseFileContent, mime_type , &error_code) )
				return ( ErrorResponse( error_code ) );
			return this->formatResponse( responseFileContent, HTTP_OK, mime_type );
		}
		else {
			if ( !this->ReadFile( filepath.c_str(), responseFileContent, mime_type , &error_code) )
				return ( ErrorResponse( error_code ) );
			return this->formatResponse( responseFileContent, HTTP_OK, mime_type );
		}
	}
	return ErrorResponse( error_code );
}
