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

#include "Request.hpp"
#include "HttpStatusCodes.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "Colors.hpp"
#include "Hashed.hpp"

#define SPACE << " "

typedef std::string string;

#define MAX_EVENTS 64

#define DEFAULT_PATH	"./static_pages"

// Utils
unsigned long	hashdjb2(const char *str);
int				setNonBlocking(int fd);
const char		*InternalERROR();

#endif