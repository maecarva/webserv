#include <iostream>
#include <string>
#include <fstream>
#include <iostream>

int main( void )
{
	std::string line;

	std::ifstream infile( "input.txt" );
	std::getline( infile, line );
	std::cout << line;

	return ( 0 );
}
