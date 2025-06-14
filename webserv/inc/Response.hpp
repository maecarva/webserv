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

    std::string		BuildResponse();
	std::string		ErrorResponse(int code);
	std::string		formatResponse(std::string& responseFileContent, int responseCode, std::string& mimetype);
	std::string		formatRedirectResponse();

	bool			ReadFile(const char *path, std::string& resultfile, std::string& mimetype);
	std::string		getMIMEtype();


    // * Getters / setters
    Request&  getRequest();
};


bool   			checkMatchingMethod(Response& res, Route *route);
std::string		HttpMessageByCode(int code);
