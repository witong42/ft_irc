#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdMode(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "MODE"));
		return false;
	}
	std::string target = argument[1];

	Channel *channel = findChannel(target);
	if (channel)
	{
		std::string modes = "";
		if (argument.size() > 2)
			modes = argument[2];
		std::vector<std::string> modeArgs;
		for (size_t i = 3; i < argument.size(); i++)
		{
			modeArgs.push_back(std::string(argument[i]));
		}
		channel->mode(_current_client, modes, modeArgs);
	}
	return true;
}
