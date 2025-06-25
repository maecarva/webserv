#pragma once

#include "Webserv.hpp"

class Client
{
private:
	int 			_fd;
	bool 			_allRead;
	size_t			_header_len;
	size_t			_contentLength;
	std::string 	_giveHeadAndBody;

	bool        	_headerParsed;
	size_t      	_bodyStart;
	bool			_isChunked;
	size_t			_endOfMessage;
	long			_maxBodyCount;
	std::string		_outBuffer;
	size_t			_outOffset;
	bool			keepalive;
public:
	Client();
	Client( int fd, long maxBodyCount);
	Client( const Client& client);
	Client& operator=( const Client& client);
	~Client();

	bool getAllRead() const;
	size_t getContentLength() const;
	std::string getThatBody();
	long getMaxBodySize();

	void	setContentLength(size_t size);
	void	setAllRead( bool a );

	std::string&	getOutBuffer();
	size_t			getOutOffset();
	void	setOutBuffer(std::string str);
	void			setOutOffset(size_t size);
	void			addOutOffset(size_t n);
	bool			isKeepAlive();
	void			setKeepAlive(bool state);

	void addBodyCount( const char *buf, ssize_t count );
};


