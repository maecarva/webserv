#include "Webserv.hpp"


Client::Client( ) : _fd(0 ), _allRead( false ), _contentLength( 0 )
{
}

Client::Client( int fd) : _fd( fd ), _allRead( false ), _contentLength( 0 )
{
}

Client::Client( const Client& client) : _fd(client._fd), _allRead(client._allRead), _header_len(client._header_len), _contentLength(client._contentLength)
, _giveHeadAndBody(client._giveHeadAndBody) {}

Client& Client::operator=( const Client& client) {
	if (this != &client) {
		_fd = client._fd;
		_allRead = client._allRead;
		_header_len = client._header_len;
		_contentLength = client._contentLength;
		_giveHeadAndBody = client._giveHeadAndBody;
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


void Client::addBodyCount( const char *buf, ssize_t count )
{
	size_t pos;

	if ( _giveHeadAndBody.size() > 0 ) // y'a deja le header.
		pos = _giveHeadAndBody.find( "\r\n\r\n" ) + 4; // find retruns index of start donc + 4 pour skip et acceder au body
	else
		pos = 0;

	for ( ssize_t i = 0; i < count; ++i )
		_giveHeadAndBody.push_back(buf[i]);

	// DEBUG(3);
	// std::cout << "Bingo bango boom: " << "size: " << _giveHeadAndBody.size() << " and " << "pos: " << pos << " start: " << (_giveHeadAndBody.size() - pos) << " != " << _contentLength << std::endl;
	if ( _giveHeadAndBody.size() - pos >= _contentLength )
	{
		_allRead = true;
		// DEBUG(4);
	}
}

// void Client::addBodyCount(const char *buf, ssize_t count)
// {
//     // Ajouter les nouvelles données
//     for (ssize_t i = 0; i < count; ++i)
//         _giveHeadAndBody += buf[i];
    
//     // Trouver la position du début du body
//     size_t headerEndPos = _giveHeadAndBody.find("\r\n\r\n");
//     if (headerEndPos == std::string::npos)
//     {
//         // Header pas encore complet
//         return;
//     }
    
//     size_t bodyStartPos = headerEndPos + 4;
//     size_t currentBodySize = _giveHeadAndBody.size() - bodyStartPos;
    
//     std::cout << "Bingo bango boom: " << "total size: " << _giveHeadAndBody.size() 
//               << " body start pos: " << bodyStartPos 
//               << " current body size: " << currentBodySize 
//               << " expected: " << _contentLength << std::endl;
    
//     // Vérifier si on a reçu tout le body
//     if (currentBodySize >= _contentLength)
//     {
//         _allRead = true;
//         // Optionnel : tronquer si on a reçu trop de données
//         if (currentBodySize > _contentLength)
//         {
//             _giveHeadAndBody.resize(bodyStartPos + _contentLength);
//         }
//     }
// }

// void Client::addBodyCount( const char *buf, ssize_t count )
// {
// 	if ( _giveHeadAndBody.size() > 0 )
// 	{
// 		size_t pos = _giveHeadAndBody.find( "\r\n\r\n" );

// 		for ( ssize_t i = 0; i < count; ++i )
// 		{
// 			_giveHeadAndBody.push_back(buf[i]);
// 		}
// 		if (_giveHeadAndBody.size() - pos  >= _contentLength)
// 		{
// 			std::cout << "allread\n";
// 			_allRead = true;
// 		}

// 	}
// 	else {
// 		for (ssize_t i = 0; i < count; ++i )
// 		{
// 			_giveHeadAndBody.push_back(buf[i]);
// 		}
// 		if (_giveHeadAndBody.size()  >= _contentLength)
// 		{
// 			std::cout << "allread\n";
// 			_allRead = true;
// 		}
// 	}
// }
