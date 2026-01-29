/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcTryRegisterClient.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 11:03:04 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/29 12:29:39 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::TryRegisterClient(Server &server)
{
	if (!_current_client)
		return false;
	if (_current_client->isPwdOk() == false)
		return false;
	if (_current_client->getNickname().empty() || _current_client->getUsername().empty())
		return false;
	if (_current_client->isRegistered())
		return true;

	_current_client->setRegistered(true);
	std::string nick = _current_client->getNickname();
	std::string user = _current_client->getUsername();
	std::string host = _current_client->getIp();
	std::string servername = server.getServerName();

	_current_client->reply(RPL_WELCOME(nick, user, host, servername));
	_current_client->reply(RPL_YOURHOST(nick, servername));
	_current_client->reply(RPL_CREATED(nick, servername));
	_current_client->reply(RPL_MYINFO(nick, servername));
	return true;
}
