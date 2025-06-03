#include "Request.hpp"

Request::Request() : 
    method(),
    route(),
    headers(),
    header_len()
{};

t_METHOD    getMethod(std::string& token) {
    return GET;

    (void)token;
}

Request::Request(const char *req) {
    std::string request(req);
    std::string token;
    std::istringstream iss(request);
    int i = 0;
    
    while (iss >> token)
    {
        if (i == 0 && (token == "GET" || token == "POST" || "DELETE"))
            this->method = getMethod(token);
        i++;
    }
};

Request::~Request() {};