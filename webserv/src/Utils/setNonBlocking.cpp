#include "Webserv.hpp"

int setNonBlocking(int fd)
{
	// fcntl allow manipulation of fd
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK); // set fd to be non blocking
}