/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCmdPass.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:42:55 by jegirard          #+#    #+#             */
/*   Updated: 2026/02/02 12:09:55 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdPass(std::vector<String> argument, Server &server)
{
	(void)server;

	if (!checkParams(argument.size(), 2, "PASS"))
		return false;

	if (_current_client->isRegistered())
	{
		_current_client->reply(ERR_ALREADYREGISTERED(_current_nick));
		return true;
	}

	if (server.checkPassword(argument[1], _current_client))
	{
		TryRegisterClient(server);
	}
	else
	{
		_current_client->reply(ERR_PASSWDMISMATCH(_current_nick) + "\r\n");
		_current_client->setShouldDisconnect(true);
		return false;
	}
	return true;
}
