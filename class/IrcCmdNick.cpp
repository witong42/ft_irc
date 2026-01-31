/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCmdNick.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:42:46 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/31 09:42:47 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdNick(std::vector<String> argument, Server &server)
{
	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NONICKNAMEGIVEN(_current_nick));
		return false;
	}

	String nick = argument[1];
	if (nick.empty() || nick.length() > 30 || !isalnum(nick[0]))
	{
		_current_client->reply(ERR_ERRONEUSNICKNAME(_current_nick, nick));
		return false;
	}
	if (server.findConnectedByNickname(nick) != NULL)
	{
		_current_client->reply(ERR_NICKNAMEINUSE(_current_nick, nick));
		return false;
	}

	_current_client->setNickname(nick);
	TryRegisterClient(server);
	return true;
}
