// #include "Webserv.hpp"

#include <string>
#include <map>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>

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
std::map<std::string, std::string> extractDataFromChunkedBody( std::string oldBody )
{
	std::map<std::string, std::string> bodyData;
	std::vector<std::string> explodedData = splitFromChain( oldBody, "--------------------------668049539896908373084707" );

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
		end = explodedData[i].find( "\0" ) - 1;
		
		value = explodedData[i].substr( start, end - start );

		bodyData[name] = value;
	}

	return ( bodyData );
}

int main( void )
{
	std::string body;

	body = "----------------------------668049539896908373084707\nContent-Disposition: form-data; name=\"username\"\r\n\r\nadmin\n----------------------------668049539896908373084707\nContent-Disposition: form-data; name=\"password\"\r\n\r\neliolebg\n----------------------------668049539896908373084707--";

	std::map<std::string, std::string> data = extractDataFromChunkedBody( body );

	for ( std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); ++it )
	{
		std::cout << it->first << " " << it->second << std::endl;

	}
}
