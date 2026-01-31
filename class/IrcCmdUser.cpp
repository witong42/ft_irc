/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCmdUser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:43:21 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/31 09:43:22 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdUser(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!checkParams(argument.size(), 5, "USER"))
		return false;

	if (_current_client->isRegistered())
	{
		_current_client->reply(ERR_ALREADYREGISTERED(_current_nick));
		return true;
	}
	_current_client->setUsername(argument[1]);
	TryRegisterClient(server);
	return true;
}
