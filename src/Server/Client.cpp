#include "Webserv.hpp"


Client::Client( ) : _fd(0 ), _allRead( false ), _contentLength( 0 ), _headerParsed(false), _bodyStart(0), _isChunked(false),
_endOfMessage( 0 ), _maxBodyCount( 0 ), _outBuffer(), _outOffset(0), keepalive(false)
{
}

Client::Client( int fd, long maxBodyCount) : _fd( fd ), _allRead( false ), _contentLength( 0 ), _headerParsed(false), _bodyStart(0), _isChunked(false),
_endOfMessage( 0 ), _maxBodyCount( maxBodyCount ), _outBuffer(), _outOffset(0), keepalive(false)
{
}

Client::Client( const Client& client) : _fd(client._fd), _allRead(client._allRead), _header_len(client._header_len), _contentLength(client._contentLength)
, _giveHeadAndBody(client._giveHeadAndBody), _headerParsed(client._headerParsed), _bodyStart(client._bodyStart), _isChunked(client._isChunked),
_endOfMessage( client._endOfMessage ), _maxBodyCount (client._maxBodyCount), _outBuffer(client._outBuffer), _outOffset(client._outOffset), keepalive(client.keepalive) {}

Client& Client::operator=( const Client& client) {
	if (this != &client) {
		_fd = client._fd;
		_allRead = client._allRead;
		_header_len = client._header_len;
		_contentLength = client._contentLength;
		_giveHeadAndBody = client._giveHeadAndBody;
		_headerParsed = client._headerParsed;
		_bodyStart = client._bodyStart;
		_isChunked = client._isChunked;
		_endOfMessage = client._endOfMessage;
        _maxBodyCount = client._maxBodyCount;
		_outBuffer = client._outBuffer;
		_outOffset = client._outOffset;
		keepalive = client.keepalive;
	}
	return *this;
}

Client::~Client()
{
}

bool Client::getAllRead() const { return ( _allRead ); }

size_t Client::getContentLength() const { return ( _contentLength ); }

std::string Client::getThatBody() { return ( _giveHeadAndBody ); };

void	Client::setContentLength(size_t size) { _contentLength = size; };
void	Client::setAllRead( bool a ) { _allRead = a; }

long    Client::getMaxBodySize() {
    return this->_maxBodyCount;
}

void Client::addBodyCount(const char* buf, ssize_t count) {
    _giveHeadAndBody.append(buf, count);
    if (!_headerParsed) {
        size_t hpos = _giveHeadAndBody.find("\r\n\r\n");
        if (hpos != std::string::npos) {
            _headerParsed = true;
            _bodyStart    = hpos + 4;
            _isChunked    = (_giveHeadAndBody.find("Transfer-Encoding: chunked") != std::string::npos);
        }
    }
    if (_headerParsed) {
        if (_isChunked) {
            size_t endChunk = _giveHeadAndBody.find("0\r\n\r\n", _bodyStart);
            if (endChunk != std::string::npos) {
                _allRead       = true;
                _endOfMessage  = endChunk + 5;  // 5 == longueur de "0\r\n\r\n"
            }
        } else {
            size_t bodyLen = _giveHeadAndBody.size() - _bodyStart;
            if (bodyLen >= _contentLength) {
                _allRead      = true;
                _endOfMessage = _giveHeadAndBody.size();
            }
        }
 	}
}


std::string&	Client::getOutBuffer() {
	return this->_outBuffer;
}

size_t			Client::getOutOffset() {
	return this->_outOffset;
}


void			Client::addOutOffset(size_t n) {
	this->_outOffset += n;
}

bool	Client::isKeepAlive() {
	return this->keepalive;
}


void			Client::setKeepAlive(bool state) {
	this->keepalive = state;
}


void	Client::setOutBuffer(std::string str) {
	this->_outBuffer = str;
}

void			Client::setOutOffset(size_t size) {
	this->_outOffset = size;
}
