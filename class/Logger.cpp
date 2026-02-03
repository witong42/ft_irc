/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:43:37 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/31 09:43:38 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Logger.hpp"

void Logger::log(const std::string &level, const std::string &message)
{
	std::time_t result = std::time(NULL);
	char timestamp[20];
	struct tm *timeinfo = std::localtime(&result);

	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

	std::ostream &os = (level == "ERROR") ? std::cerr : std::cout;
	os << "[" << timestamp << "] [" << level << "] " << message << std::endl;
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
