#ifndef LOGGER
#define LOGGER

class Logger
{
private:
	Logger(/* args */);
	~Logger();
public:
	static	void	debug(const char *message); // BLUE
	static	void	info(const char *message); // GREEN
	static	void	warn(const char *message); // YELLOW
	static	void	error(const char *message); // RED
};



#endif