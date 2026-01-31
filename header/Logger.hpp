/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:44:12 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/31 09:44:13 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
