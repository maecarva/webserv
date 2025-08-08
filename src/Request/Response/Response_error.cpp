#include "Response.hpp"
#include "Webserv.hpp"
#include <sstream>

std::string	HttpMessageByCode(int code) {
	std::string message = "status code";

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


std::string formatErrorPage(int code) {
	std::ostringstream			resp;

	resp << "HTTP/1.1 " << code << " " << HttpMessageByCode(code) << "\r\n";
	resp << "Content-Length: " << 130 << "\r\n";
	resp << "Content-Type: text/html" << "\r\n\r\n";
	resp << "<!DOCTYPE html><html><head><title>Status ";
	resp << code << "</title></head><body><h1>Status ";
	resp << code << " Page</h1><h2>Default Error page</h2></body></html>";

	return resp.str();
}

std::string	Response::ErrorResponse(int code) {
	const Request&              req                 = this->getRequest();
    Server&                     current_server      = req.getServer();
    Config&                     current_config      = current_server.getConfig();
    std::map<int, std::string>  error_pages         = current_config.getErrorPages();
    std::string                 page                = "";
	std::ostringstream			resp;
	std::string					mime_type			= "";
	int							errorcode			= HTTP_OK;

	this->getRequest().setResponseCode(code);
	if (current_config.getErrorPages().count(code) != 0)
	{
		if (!ReadFile(current_config.getErrorPages()[code].c_str(), page, mime_type, &errorcode))
			return formatErrorPage(500);

		resp << "HTTP/1.1 " << code << " " << HttpMessageByCode(code) << "\r\n";
		resp << "Content-Length: " << page.size() << "\r\n";
		resp << "Content-Type: text/html" << "\r\n\r\n";
		resp << page;
		return resp.str();
	}
	else if (!ReadFile(current_config.getErrorPages()[code].c_str(), page, mime_type, &errorcode))
		return formatErrorPage(code);

	return resp.str();
}
