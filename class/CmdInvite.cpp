#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdInvite(std::vector<String> argument, Server &server)
{
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 3)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "INVITE"));
		return false;
	}

	std::string targetNick = argument[1];
	std::string channelName = argument[2];

	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		_current_client->reply(ERR_NOSUCHCHANNEL(_current_nick, channelName));
		return false;
	}

	Client *target = server.findConnectedByNickname(targetNick);
	if (!target)
	{
		_current_client->reply(ERR_NOSUCHNICK(_current_nick, targetNick));
		return false;
	}

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
