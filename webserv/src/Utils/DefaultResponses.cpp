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

std::string	formatDirectoryListing(std::vector<std::string>& files) {
	std::ostringstream resp;
	std::ostringstream resp2;
	size_t filesizes = 0;

	for (std::vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		filesizes += (*it).size();
	}
	resp << "HTTP/1.1 200 OK\r\nContent-Length: ";

	resp2 << "<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Directory</title><ul>";
	for (std::vector<string>::iterator it = files.begin(); it != files.end(); it++)
		resp2 << "<li>" << *it;
	resp2 << "</ul>";

	resp << resp2.str().size() << "\r\n\r\n" << resp2.str();
	//std::cout <<  "RESP:\n" << resp.str() << std::endl;
	return resp.str();
}
