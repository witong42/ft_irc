#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>
#include <map>

bool Irc::CmdPrivmsg(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!checkRegistered())
		return false;

	if (!checkParams(argument.size(), 2, "PRIVMSG"))
		return false;

	std::string target = argument[1];
	Channel *channel = getChannelOrError(target);
	if (!channel)
		return false; // Error already sent by getChannelOrError

	if (!_current_client->getNickname().empty())
	{
		std::string message = extractMessage(argument, 2);
		std::string msg = ":" + _current_nick + " PRIVMSG " + target + " :" + message;
		channel->broadcast(msg, _current_client);
	}
	return true;
}
