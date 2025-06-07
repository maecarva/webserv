#include "Webserv.hpp"

const char	*InternalERROR(Request& request) {
	request.setError(500, -1);
	return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 178\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Internal Server Error</title><h1>Internal Server Error</h1>";
}

const char	*ERROR_404(Request& request) {
	request.setError(404, -1);
	return "HTTP/1.1 404 Error\r\nContent-Length: 154\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Error 404</title><h1>Error 404</h1>";
}

const char	*BadRequest(Request& request) {
	request.setError(400, -1);
	return "HTTP/1.1 400 Bad Request\r\nContent-Length: 158\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Bad Request</title><h1>Bad Request</h1>";
}