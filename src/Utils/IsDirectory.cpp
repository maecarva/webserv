#include "Webserv.hpp"

int is_directory(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return !S_ISREG(path_stat.st_mode);
}