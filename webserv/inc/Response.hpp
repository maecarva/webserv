#pragma once

#include "Webserv.hpp"

class Response
{
private:
    const Request&      _req;
    std::string         _default_response;
    std::ostringstream  _res;
    std::string         _response_file;
    std::string         _response_file_ext;

public:
    Response(const Request& req);
    ~Response();

    const char      *BuildResponse();
    bool            CheckErrors();



    // * Getters / setters
    const Request&  getRequest();
    const char      *getDefaultResponse();
};
