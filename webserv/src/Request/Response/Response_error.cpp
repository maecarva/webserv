#include "Response.hpp"
#include "Webserv.hpp"
#include <sstream>

std::string	HttpMessageByCode(int code) {
	std::string message = "Invalid error code";

	switch (code)
	{
	case 200:
		message = HTTP_MESSAGE_OK;
		break;
	case 302:
		message = HTTP_MESSAGE_FOUND;
		break;
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
	std::ostringstream			resp;
	std::string					mime_type			= "";

    this->getRequest().setResponseCode(code);
	if (current_config.getErrorPages()[code] == "")
		goto SWITCH;
	if (!ReadFile(current_config.getErrorPages()[code].c_str(), page, mime_type))
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

SWITCH:

	std::string resp1;
	const char* respeuh;
    switch (code)
	{
	case 500:
		respeuh = InternalERROR();
		break;
	case 400:
		respeuh = BadRequest();
		break;
    case 404:
		respeuh = ERROR_404();
		break;
    case 405:
		respeuh = MethodNotAllowed();
		break;
	default:
		respeuh = InternalERROR();
		break;
	}

	resp1 = respeuh;
	if (strcmp(this->getRequest().getMethod(), "HEAD") == 0)
	{
		size_t pos = resp1.find("\r\n\r\n") + 4;
		resp1.erase(pos, resp1.size() - pos);
	}
	std::cout << resp1 << std::endl;
	return resp1.c_str();
}
