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
	if (!checkRegistered())
		return false;

	if (!checkParams(argument.size(), 2, "PART"))
		return false;

	std::string channelName = argument[1];
	Channel *channel = getChannelOrError(channelName);
	if (!channel)
		return false;

	if (!channel->hasUser(_current_client))
	{
		_current_client->reply(ERR_NOTONCHANNEL(_current_nick, channelName));
		return false;
	}
	std::string reason = extractMessage(argument, 2);

	std::string msg = ":" + _current_nick + "!" + _current_client->getUsername() + "@" + _current_client->getIp() + " PART " + channelName;
	if (!reason.empty())
		msg += " :" + reason;

	channel->broadcast(msg);
	channel->removeUser(_current_client);

	if (channel->getUserCount() == 0)
	{
		removeChannel(channelName);
	}
	return true;
}
