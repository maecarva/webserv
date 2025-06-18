// #include "Webserv.hpp"

#include <string>
#include <map>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>
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

//Request::
std::map<std::string, std::string> Request::extractDataFromChunkedBody( std::string oldBody )
{
	std::map<std::string, std::string> bodyData;
	std::vector<std::string> explodedData = splitFromChain( oldBody, "--" + _chunkedLimiter );

	for ( size_t i = 1; i < explodedData.size() - 1; ++i )
	{
		std::string name;
		std::string value;
		size_t start;
		size_t end;

		start = explodedData[i].find( "name=\"" ) + 6; // On skip name="
		end = explodedData[i].find( "\r\n\r\n" ) - 1; // On arrive a a fin -> ", on ne prend que jusqu'au dernier char donc -2.

		name = explodedData[i].substr( start, end - start );

		start = explodedData[i].find( "\r\n\r\n" ) + 4; // On skip name="
		end = explodedData[i].find_last_of( "\n" ); // se termine par un /n

		value = explodedData[i].substr( start, end - start );

		bodyData[name] = value;
	}

	return ( bodyData );
}
