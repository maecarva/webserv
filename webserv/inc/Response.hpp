#pragma once

#include "Webserv.hpp"

class Route;

class Response
{
private:
    Request&      _req;
    std::string         _default_response;
    std::ostringstream  _res;
    std::string         _response_file;
    std::string         _response_file_ext;

public:
    Response(Request& req);
    ~Response();

    const char      *BuildResponse();
    bool            CheckErrors();
    const char      *ErrorResponse(int code);



    // * Getters / setters
    Request&  getRequest();
    const char      *getDefaultResponse();
};


bool    checkMatchingRoutes(std::vector<Route>& routes, Response& res, Route **route, bool *indexRequested);
bool    checkMatchingMethod(Response& res, Route *route);