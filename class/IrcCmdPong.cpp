/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCmdPong.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:43:03 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/31 09:43:04 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include "../header/Logger.hpp"
#include <vector>

bool Irc::CmdPong(std::vector<String> argument, Server &server)
{
	Logger::debug("Received PONG command " + server.getServerName());

	if (!checkRegistered())
		return false;

	if (!checkParams(argument.size(), 2, "PONG"))
		return false;
	return true;
}
