#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdKick(std::vector<String> argument, Server &server)
{
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 3)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "KICK"));
		return false;
	}

	std::string channelName = argument[1];
	std::string targetNick = argument[2];
	std::string reason = _current_nick;

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

	if (!channel->isOperator(_current_client))
	{
		_current_client->reply(ERR_CHANOPRIVSNEEDED(_current_nick, channelName));
		return false;
	}

	Client *target = server.findConnectedByNickname(targetNick);
	if (!target || !channel->hasUser(target))
	{
		_current_client->reply(ERR_USERNOTINCHANNEL(_current_nick, targetNick, channelName));
		return false;
	}
	channel->kick(_current_client, targetNick, reason);
	return true;
}
