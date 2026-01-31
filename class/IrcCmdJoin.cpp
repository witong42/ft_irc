/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCmdJoin.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 09:42:18 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/31 09:42:19 by jegirard         ###   ########.fr       */
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

// #join toto1m,toot2,toto3 key1,,key3/r/n
// join toto2,toto3 key2,key3/r/n
//

bool Irc::CmdJoin(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!checkRegistered())
		return false;

	if (!checkParams(argument.size(), 2, "JOIN"))
		return false;

	std::vector<String> channels = argument[1].split(",");
	std::vector<String> keys;
	if (argument.size() > 2)
		keys = argument[2].split(",");

	for (size_t i = 0; i < channels.size(); i++)
	{
		std::string channelName = std::string(channels[i]);
		std::string channelKey = "";
		if (i < keys.size())
			channelKey = std::string(keys[i]);

		if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
		{
			_current_client->reply(ERR_NOSUCHCHANNEL(_current_nick, channelName));
			continue;
		}

		std::map<String, Channel *>::iterator it = _channels.find(channels[i]);
		Channel *channel = NULL;
		if (it != this->_channels.end())
		{
			channel = it->second;
			if (channel->hasUser(_current_client))
				continue;

			// key check (+k)
			if (channel->hasKey() && channelKey != channel->getKey())
			{
				_current_client->reply(ERR_BADCHANNELKEY(_current_nick, channelName));
				continue;
			}
			
			// invite-only check (+i)
			if (channel->isInviteOnly() && !channel->hasInvite(_current_client))
			{
				_current_client->reply(ERR_INVITEONLYCHAN(_current_nick, channelName));
				continue;
			}

			// limit check (+l)
			if (channel->hasLimit())
			{
				int limit = std::atoi(channel->getLimit().c_str());
				if (limit > 0 && static_cast<int>(channel->getUserCount()) >= limit)
				{
					_current_client->reply(ERR_CHANNELISFULL(_current_nick, channelName));
					continue;
				}
			}
			channel->addUser(_current_client);
		}
		else
		{
			channel = new Channel(channelName, _current_client);
			if (!channelKey.empty())
				channel->setKey(channelKey);
			this->_channels[channels[i]] = channel;
		}

		std::string joinMsg = ":" + _current_nick + "!" + _current_client->getUsername() + "@" + _current_client->getIp() + " JOIN " + channelName;
		channel->broadcast(joinMsg);

		std::string topic = channel->getTopic();
		if (!topic.empty())
			_current_client->reply(RPL_TOPIC(_current_nick, channelName, topic));
		else
			_current_client->reply(RPL_NOTOPIC(_current_nick, channelName));

		std::string userList = channel->getUserList();
		_current_client->reply(RPL_NAMREPLY(_current_nick, channelName, userList));
		_current_client->reply(RPL_ENDOFNAMES(_current_nick, channelName));
	}
	return true;
}
