/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCmdInvite.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:42:12 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/31 09:42:13 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdInvite(std::vector<String> argument, Server &server)
{
	if (!checkRegistered())
		return false;

	if (!checkParams(argument.size(), 3, "INVITE"))
		return false;

	std::string targetNick = argument[1];
	std::string channelName = argument[2];

	Channel *channel = getChannelOrError(channelName);
	if (!channel)
		return false;

	if (!channel->hasUser(_current_client))
	{
		_current_client->reply(ERR_NOTONCHANNEL(_current_nick, channelName));
		return false;
	}

	if (channel->isInviteOnly() && !channel->isOperator(_current_client))
	{
		_current_client->reply(ERR_CHANOPRIVSNEEDED(_current_nick, channelName));
		return false;
	}

	Client *target = getClientOrError(server, targetNick);
	if (!target)
		return false;

	if (channel->hasUser(target))
	{
		_current_client->reply(ERR_USERONCHANNEL(_current_nick, targetNick, channelName));
		return false;
	}

	channel->invite(target);
	std::string inviteMsg = ":" + _current_nick + " INVITE " + targetNick + " :" + channelName;
	if (target)
		target->reply(inviteMsg);

	_current_client->reply(RPL_INVITING(_current_nick, channelName, targetNick));

	return true;
}
