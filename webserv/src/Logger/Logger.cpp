#include "Webserv.hpp"
#include "Logger.hpp"
#include <ctime>
#include <iostream>
#include <sstream>


Logger::Logger() {}

Logger::~Logger() {}


void	print_digit_date(int d)
{
	if (d < 10)
		std::cout << "0";
	std::cout << d;
}

void	printTimestamp(void) {
	time_t timestamp;
	time(&timestamp);
	struct tm datetime = *localtime(&timestamp);

	std::cout << "[";
	print_digit_date(datetime.tm_mday);
	std::cout << "-";
	print_digit_date(datetime.tm_mon);
	std::cout << "-";
	std::cout << datetime.tm_year + 1900;
	std::cout << " ";
	print_digit_date(datetime.tm_hour);
	std::cout << ":";
	print_digit_date(datetime.tm_min);
	std::cout << ":";
	print_digit_date(datetime.tm_sec);
	std::cout << "]";
}


void Logger::debug(const char *message) {
	PRINTC(BLU, "[DEBUG] ");
	printTimestamp();
	std::cout << " " << message << std::endl;
}


void Logger::info(const char *message) {
	PRINTC(GRN, "[INFO] ");
	printTimestamp();
	std::cout << " " << message << std::endl;
}


void Logger::warn(const char *message) {
	PRINTC(YEL, "[WARNING] ");
	printTimestamp();
	std::cout << " " << message << std::endl;
}


void Logger::error(const char *message) {
	PRINTC(RED, "[ERROR] ");
	printTimestamp();
	std::cout << " " << message << std::endl;
}