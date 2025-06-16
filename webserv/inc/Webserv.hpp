#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <ostream>
#include <fstream>

#include <csignal>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <exception>
#include <map>
#include <vector>
#include <sys/epoll.h>
#include <sys/wait.h>

#include "HttpStatusCodes.hpp"
#include "HttpStatusMessages.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "Colors.hpp"
#include "Hashed.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Route.hpp"

#define SPACE << " "
#define NEWLINE (std::cout << std::endl)

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

typedef std::string string;

#define MAX_EVENTS 64

#define DEFAULT_PATH	"/home/ebonutto/Documents/webserv/webserv/static_pages"
#define DEFAULT_INDEX	"/index.html"
#define CLOSE false
#define KEEP_ALIVE true

// Utils
unsigned long	hashdjb2(const char *str);
int				setNonBlocking(int fd);
int				is_directory(const char *path);
bool			getAllFilesFromDirectory(std::vector<string>& files, const char *path);

// default error pages
const char		*InternalERROR();
const char		*ERROR_404();
const char		*BadRequest();

std::string		formatDirectoryListing(std::vector<std::string>& files);

std::vector<Server>	CreateServerPool(std::vector<Config>& configs);

// cgi
void executeCgi( std::vector< std::string > command, char **envp, const std::string& input );

#endif