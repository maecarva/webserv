#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <sstream>

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

#include "Request.hpp"
#include "HttpStatusCodes.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "Colors.hpp"


typedef std::string string;

#define MAX_EVENTS 64
#define PORT 3000

// Utils
unsigned long	hashdjb2(unsigned char *str);
int				setNonBlocking(int fd);

#endif