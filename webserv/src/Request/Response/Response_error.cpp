#include "Response.hpp"
#include "Webserv.hpp"
#include <sstream>

// TODO :
// check if error page is set
// return default if not
bool    Response::CheckErrors() {
    const Request&              req                 = this->getRequest();
    int                         set_response_code   = req.getResponseCode();
    Server&                     current_server      = req.getServer();
    Config&                     current_config      = current_server.getConfig();
    std::map<int, std::string>  error_pages         = current_config.getErrorPages();
    std::string                 page                = "";

    if (set_response_code != 200)
        page = error_pages[set_response_code];
    if (page != "")
    {
		this->_default_response =  page;
        return true;
    }

    switch (set_response_code)
	{
	case 500:
		this->_default_response =  std::string(InternalERROR());
        return true;
	case 400:
		this->_default_response =  std::string(BadRequest());
        return true;
    case 404:
		this->_default_response =  std::string(ERROR_404());
        return true;
	default:
		break;
	}
    return false;
}

std::string	HttpMessageByCode(int code) {
	std::string message = "Invalid error code";

	switch (code)
	{
	case 404:
		message =  HTTP_MESSAGE_NOT_FOUND;
		break;
	case 500:
		message =  HTTP_MESSAGE_INTERNAL_SERVER_ERROR;
		break;
	case 400:
		message =  HTTP_MESSAGE_BAD_REQUEST;
		break;
	default:
		break;
	}
	return message;
}

// TODO : respond with error pages
std::string	Response::ErrorResponse(int code) { 
	const Request&              req                 = this->getRequest();
    Server&                     current_server      = req.getServer();
    Config&                     current_config      = current_server.getConfig();
    std::map<int, std::string>  error_pages         = current_config.getErrorPages();
    std::string                 page                = "";
	std::ostringstream resp;

    this->getRequest().setResponseCode(code);
	if (!ReadFile(current_config.getErrorPages()[code].c_str(), page))
	{
		return InternalERROR();
	}
	else {
		resp << "HTTP/1.1 " << code << " " << HttpMessageByCode(404) << "\r\n";

		resp	<< "Content-Length: " << page.length() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n"
				<< page;
		return resp.str();
		std::cout << resp.str() << std::endl;

	}
    switch (code)
	{
	case 500:
		return InternalERROR();
	case 400:
		return BadRequest();
    case 404:
		return ERROR_404();
	default:
		return InternalERROR();
		break;
	}
}
