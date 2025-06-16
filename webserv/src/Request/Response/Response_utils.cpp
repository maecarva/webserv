#include "Webserv.hpp"

bool    checkMatchingMethod(Response& res, Route *route) {

    std::vector<std::string>	methods = route->getAllowedMethods();
    std::string					res_method = std::string(res.getRequest().getMethod());

    std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), res.getRequest().getMethod());
	return (it == methods.end() ? false : true);
}

bool	Response::ReadFile(const char *path, std::string& resultfile, std::string& mimetype)
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
				return false;
		}

		inputfile.open(path, std::ios::in);
		if (inputfile.fail())
		{
			std::cerr << "Unable to open " << path << std::endl;
			return false;
		}
		std::string content((std::istreambuf_iterator<char>(inputfile)), std::istreambuf_iterator<char>());
		resultfile = content;
		return true;
	}
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