#include "Webserv.hpp"


Client::Client( ) : _fd(0 ), _allRead( false ), _contentLength( 0 )
{
}

Client::Client( int fd) : _fd( fd ), _allRead( false ), _contentLength( 0 )
{
}

Client::Client( const Client& client) : _fd(client._fd), _allRead(client._allRead), _header_len(client._header_len), _contentLength(client._contentLength)
, giveHeadAndBody(client.giveHeadAndBody) {}

Client& Client::operator=( const Client& client) {
	if (this != &client) {
		_fd = client._fd;
		_allRead = client._allRead;
		_header_len = client._header_len;
		_contentLength = client._contentLength;
		giveHeadAndBody = client.giveHeadAndBody;
	}
	return *this;
}

Client::~Client()
{
}

bool Client::getAllRead() const { return ( _allRead ); }

size_t Client::getContentLength() const { return ( _contentLength ); }

std::string Client::getThatBody() { return ( giveHeadAndBody ); };

void	Client::setContentLendht(size_t size) { _contentLength = size; };
void	Client::setAllRead( bool a ) { _allRead = a; }


void Client::addBodyCount( const char *buf, ssize_t count )
{
	if (giveHeadAndBody.size() > 0)
	{
		size_t pos = giveHeadAndBody.find("\r\n\r\n");

		for (ssize_t i = 0; i < count; ++i )
		{
			giveHeadAndBody.push_back(buf[i]);
		}
		if (giveHeadAndBody.size() - pos  >= _contentLength)
		{
			std::cout << "allread\n";
			_allRead = true;
		}

	}
	else {
		for (ssize_t i = 0; i < count; ++i )
		{
			giveHeadAndBody.push_back(buf[i]);
		}
		if (giveHeadAndBody.size()  >= _contentLength)
		{
			std::cout << "allread\n";
			_allRead = true;
		}
	}
}