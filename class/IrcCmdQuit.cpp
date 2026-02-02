/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCmdQuit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:43:12 by jegirard          #+#    #+#             */
/*   Updated: 2026/02/02 11:53:24 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>
#include <map>

bool Irc::CmdQuit(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!checkRegistered())
		return false;

	std::string reason = extractMessage(argument, 1);
	ircDisconnectClient(_current_client, reason);
	_current_client->reply("ERROR :Closing Link: " + _current_client->getIp() + " (" + reason + ")");
	_current_client->setShouldDisconnect(true);

	return true;
}
