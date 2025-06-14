#include "Webserv.hpp"

bool    checkMatchingMethod(Response& res, Route *route) {

    std::vector<std::string>	methods = route->getAllowedMethods();
    std::string					res_method = std::string(res.getRequest().getMethod());

    std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), res.getRequest().getMethod());
	return (it == methods.end() ? false : true);
}

bool	ReadFile(const char *path, std::string& resultfile)
{
	std::cout << path << std::endl;
	if (!path)
		return false;
	std::fstream inputfile;

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


std::string	BuildFilePath(std::string rootdir, std::string ressource) {
	if (rootdir[rootdir.size() - 1] == '/' && ressource[0] != '/')
		return rootdir + ressource;
	else if (rootdir[rootdir.size() - 1] != '/' && ressource[0] == '/')
		return rootdir + ressource;
	else
		return rootdir + "/" + ressource;
}