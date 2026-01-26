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
	if (!checkRegistered())
		return false;

	if (!checkParams(argument.size(), 2, "MODE"))
		return false;
	std::string target = argument[1];

	Channel *channel = getChannelOrError(target);
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
