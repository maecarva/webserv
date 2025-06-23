#include "Webserv.hpp"

bool	getAllFilesFromDirectory(std::vector<string>& files, const char *path)
{
	DIR*	dir = opendir(path);
	if (!dir)
		return false;

	struct dirent *entry = readdir(dir);
	while (entry != NULL)
	{
		files.push_back(entry->d_name);
		entry = readdir(dir);
	}
	closedir(dir);
	return true;
}