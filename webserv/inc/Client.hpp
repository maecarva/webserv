#pragma once

#include "Webserv.hpp"

class Client
{
private:
	int _fd;
	bool _allRead;
	size_t	_header_len;
	size_t	_contentLength;
	std::string giveHeadAndBody;

public:
	Client( int fd, int contentL );
	~Client();
	bool getAllRead() const { return ( _allRead ); }
	size_t getContentLength() const { return ( _contentLength ); }

	void addBodyCount( char *buf, ssize_t count )
	{
		for (ssize_t i = 0; i < count; i++)
		{
			giveHeadAndBody.push_back(buf[i]);
		}
		_contentLength += count;
		if (giveHeadAndBody.size() == _contentLength)
			_allRead = true;
	}
};

Client::Client( int fd, int contentL ) : _fd( fd ), _allRead( false ), _contentLength( contentL )
{
	size_t headerlen = giveHeadAndBody.find("\r\n\r\n");
	_header_len = headerlen;
}

Client::~Client()
{
}
