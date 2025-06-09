#include "Webserv.hpp"

bool	getAllFilesFromDirectory(std::vector<string>& files, const char *path)
{
	DIR*	dir = opendir(path);
	if (!dir)
		return false;

	struct dirent *entry = readdir(dir);
	while (entry != NULL)
	{
		//std::cout << "file found : " << entry->d_name << std::endl;
		files.push_back(entry->d_name);
		entry = readdir(dir);
	}
	closedir(dir);
	return true;
}