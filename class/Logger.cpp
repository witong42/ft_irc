#include "../header/Logger.hpp"

void Logger::log(const std::string &level, const std::string &message)
{
	std::time_t result = std::time(NULL);
	char timestamp[20];
	struct tm *timeinfo = std::localtime(&result);

	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

	std::cout << "[" << timestamp << "] [" << level << "] " << message << std::endl;
}

void Logger::info(const std::string &message)
{
	log("INFO", message);
}

void Logger::error(const std::string &message)
{
	log("ERROR", message);
}

void Logger::debug(const std::string &message)
{
	log("DEBUG", message);
}
