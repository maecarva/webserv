#include "Webserv.hpp"

bool    checkMatchingMethod(Response& res, Route *route) {

    std::vector<std::string>	methods = route->getAllowedMethods();
    std::string					res_method = std::string(res.getRequest().getMethod());

    std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), res.getRequest().getMethod());
	return (it == methods.end() ? false : true);
}


std::string	getMimeType(const char *path) {
	std::string pathfile = path;
	std::string ext;

	for (std::string::iterator i = pathfile.end() - 1; i != pathfile.begin(); i--)
	{
		ext.push_back(*i);
		if (*i == '.')
			break ;
	}

	std::reverse(ext.begin(), ext.end());
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
	unsigned long hash = hashdjb2(ext.c_str());
	switch (hash)
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

bool	Response::ReadFile(const char *path, std::string& resultfile, std::string& mimetype, int *errorcode)
{
	if (!path)
		return false;
	std::fstream inputfile;

	if (access(path, F_OK | R_OK) == 0) // access ok
	{
		if (is_directory(path))
		{
			if (this->getRequest().getCorrespondingRoute().getDirectoryListing())
			{
				std::vector<std::string> files;
				if (!getAllFilesFromDirectory(files, path))
					return false;
				resultfile = formatDirectoryListing(files);
				mimetype = "Content-type: text/html";
				return true;
			}
			else
			{
				*errorcode = HTTP_FORBIDDEN;
				return false;
			}
		}

		inputfile.open(path, std::ios::in);
		if (inputfile.fail())
		{
			std::cerr << "Unable to open " << path << std::endl;
			return false;
		}
		std::string content((std::istreambuf_iterator<char>(inputfile)), std::istreambuf_iterator<char>());
		resultfile = content;
		mimetype = getMimeType(path);
		return true;
	}
	else {
		Logger::error("Cannot open file");
		*errorcode = HTTP_FORBIDDEN;
		return false;
	}
	*errorcode = HTTP_INTERNAL_SERVER_ERROR;
	return false;

}


std::string	BuildFilePath(std::string rootdir, std::string ressource) {
	if (rootdir[rootdir.size() - 1] == '/' && ressource[0] != '/')
		return rootdir + ressource;
	else if (rootdir[rootdir.size() - 1] != '/' && ressource[0] == '/')
		return rootdir + ressource;
	else
		return rootdir + "/" + ressource;
}

std::string	formatDirectoryListing(std::vector<std::string>& files) {
	std::ostringstream resp2;
	std::string tmpl = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" /><meta content=\"width=device-width,initial-scale=1\" name=\"viewport\" /><title>Directory</title></head><body><ul>";
	resp2 << tmpl;
	
	for (std::vector<string>::iterator it = files.begin(); it != files.end(); it++)
		resp2 << "<li>" << *it << "</li>";
	resp2 << "</ul></body></html>";
	return resp2.str();
}