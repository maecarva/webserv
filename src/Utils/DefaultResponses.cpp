#include "Webserv.hpp"

// 500
const char	*InternalERROR() {
	return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 178\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Internal Server Error</title><h1>Internal Server Error</h1>";
}

// 404
const char	*ERROR_404() {
	return "HTTP/1.1 404 Error\r\nContent-Length: 154\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Error 404</title><h1>Error 404</h1>";
}

// 400
const char	*BadRequest() {
	return "HTTP/1.1 400 Bad Request\r\nContent-Length: 158\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Bad Request</title><h1>Bad Request</h1>";
}

const char *MethodNotAllowed() {
	return "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 172\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Method Not Allowed</title><h1>Method Not Allowed</h1>";
}

const char *PayloadTooLarge() {
	return "HTTP/1.1 413 Payload Too Large\r\nContent-Length: 170\r\n\r\n<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Payload Too Large</title><h1>Payload Too Large</h1>";
}