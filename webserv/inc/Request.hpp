#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>

typedef enum e_METHOD {
    GET,
    POST,
    DELETE
}   t_METHOD;

class Request
{
private:
    Request();

    t_METHOD method;
    std::string route;
    std::map<std::string, std::string> headers;
    int header_len;
public:
    Request(const char *req);
    ~Request();
};


#endif