#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>
#include <map>

bool Irc::CmdPart(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "PART"));
		return false;
	}

	std::string channelName = argument[1];
	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		_current_client->reply(ERR_NOSUCHCHANNEL(_current_nick, channelName));
		return false;
	}

	if (!channel->hasUser(_current_client))
	{
		_current_client->reply(ERR_NOTONCHANNEL(_current_nick, channelName));
		return false;
	}
	std::string reason = "";
	if (argument.size() > 2)
	{
		reason = argument[2];
		for (size_t i = 3; i < argument.size(); i++)
			reason += " " + argument[i];
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}

	std::string msg = ":" + _current_nick + "!" + _current_client->getUsername() + "@" + _current_client->getIp() + " PART " + channelName;
	if (!reason.empty())
		msg += " :" + reason;

	channel->broadcast(msg);
	channel->removeUser(_current_client);

	if (channel->getUserCount() == 0)
	{
		_channels.erase(channelName);
		delete channel;
	}
	return true;
}
