#include "Webserv.hpp"

const char	*InternalERROR() {
	return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 227\r\n\r\n<!doctype html>\
<html lang=\"en\">\
<head>\
  <title>500 Internal Server Error</title>\
</head>\
<body>\
  <h1>Internal Server Error</h1>\
  <p>The server was unable to complete your request. Please try again later.</p>\
</body>\
</html>";
}

const char	*BadRequest() {
	return "HTTP/1.1 400 Bad Request Error\r\nContent-Length: 227\r\n\r\n<!doctype html>\
<html lang=\"en\">\
<head>\
  <title>500 Internal Server Error</title>\
</head>\
<body>\
  <h1>Internal Server Error</h1>\
  <p>The server was unable to complete your request. Please try again later.</p>\
</body>\
</html>";
}