#include "Webserv.hpp"

std::vector<std::string> splitFromChain( std::string str, std::string chain )
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = start;

	while ( ( end = str.find( chain, start ) ) != std::string::npos )
	{
		result.push_back( str.substr( start, end - start ) );
		start = end + chain.length();
	}

	result.push_back( str.substr( start ) );
	return ( result );
}

std::map<std::string, std::string> Request::extractDataFromChunkedBody( std::string oldBody )
{
	std::map<std::string, std::string> bodyData;

	std::vector<std::string> explodedData = splitFromChain( oldBody, _chunkedLimiter );
	while (  )
	{

	}

}
