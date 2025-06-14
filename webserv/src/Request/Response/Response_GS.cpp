#include "Webserv.hpp"

const char  *Response::getDefaultResponse() {
    return this->_default_response.c_str();
}

Request&  Response::getRequest() {
    return this->_req;
}