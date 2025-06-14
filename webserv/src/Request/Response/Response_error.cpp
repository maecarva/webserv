#include "Webserv.hpp"

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


// TODO : respond with error pages
const char  *Response::ErrorResponse(int code) {
    this->getRequest().setResponseCode(code);
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