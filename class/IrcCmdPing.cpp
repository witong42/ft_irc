/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCmdPing.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:42:57 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/31 09:42:58 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdPing(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!checkRegistered())
		return false;

	if (!checkParams(argument.size(), 2, "PING"))
		return false;

	std::string server1 = std::string(argument[1]);
	std::string server2 = argument.size() >= 3 ? std::string(argument[2]) : "";

	std::string response = "PONG " + server1;
	if (!server2.empty())
		response += " " + server2;

	_current_client->reply(response);
	return true;
}
