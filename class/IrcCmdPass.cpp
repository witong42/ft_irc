/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCmdPass.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:42:55 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/31 09:42:56 by jegirard         ###   ########.fr       */
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
	// std::cout << server.getClientFd() << std::endl;
	if (!checkParams(argument.size(), 2, "PASS"))
		return false;
		
	if (server.checkPassword(argument[1], _current_client))
	{
		TryRegisterClient(server);
	}
	else
	{
		_current_client->reply(ERR_PASSWDMISMATCH(_current_nick) + "\r\n");
		return false;
	}
	return true;
}
