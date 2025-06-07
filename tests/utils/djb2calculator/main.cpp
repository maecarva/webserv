#include <fstream>
#include <iostream>
#include <sstream>

unsigned long hashdjb2(const char *str)
{
	const unsigned char	*s = (const unsigned char *)str;
    unsigned long hash = 5381;
    int c;

    while ((c = *s++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

int main(int argc, char const *argv[])
{
	if (argc != 2)
		return 1;
		
	std::fstream inputfile;
	inputfile.open(argv[1], std::ios::in);
	if (inputfile.fail())
	{
		std::cout << "Unable to open " << argv[1] << std::endl;
		return 1;
	}
	std::string str((std::istreambuf_iterator<char>(inputfile)), std::istreambuf_iterator<char>());

	
    std::string token;
    std::istringstream iss(str);

	while (iss >> token)
	{
		std::cout << "#define " << token << "_DJB2 " << "0x" << std::hex << hashdjb2(token.c_str()) << std::endl;
	}
	return 0;
}
