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
#include <sys/epoll.h>
#include <cstdlib>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h>

#include "Request.hpp"
#include "HttpStatusCodes.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "Colors.hpp"
#include "Hashed.hpp"

#define SPACE << " "
#define NEWLINE (std::cout << std::endl)

typedef std::string string;

#define MAX_EVENTS 64

#define DEFAULT_PATH	"/home/maecarva/Desktop/mongithub/webserv/webserv/static_pages"
#define DEFAULT_INDEX	"/index.html"
#define CLOSE false
#define KEEP_ALIVE true

// Utils
unsigned long	hashdjb2(const char *str);
int				setNonBlocking(int fd);
int				is_directory(const char *path);
bool			getAllFilesFromDirectory(std::vector<string>& files, const char *path);

// default error pages
const char		*InternalERROR(Request& request);
const char		*ERROR_404(Request& request);

std::string		formatDirectoryListing(std::vector<std::string>& files);
#endif