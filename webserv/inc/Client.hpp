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
	Client();
	Client( int fd);
	Client( const Client& client);
	Client& operator=( const Client& client);
	~Client();
	
	bool getAllRead() const;
	size_t getContentLength() const;
	std::string getThatBody();

	void	setContentLendht(size_t size);
	void	setAllRead( bool a );

	void addBodyCount( const char *buf, ssize_t count );
};


