#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <ctime>

class Logger
{
public:
	static void log(const std::string &level, const std::string &message);
	static void info(const std::string &message);
	static void error(const std::string &message);
	static void debug(const std::string &message);
};

#endif
