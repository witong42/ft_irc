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
	if (target.find(',') != std::string::npos)
	{
		for (size_t i = 0; i < target.size(); i++)
		{
			if (target[i] == ',')
			{
				std::string singleTarget = target.substr(0, i);
				if (singleTarget.empty())
				{
					_current_client->reply(ERR_NOSUCHNICK(_current_nick, singleTarget));
					return false;
				}
				if (singleTarget[0] == '#')
				{
					Channel *channel = getChannelOrError(singleTarget);
					if (!channel)
						return false; // Error already sent by getChannelOrError
					if (!_current_client->getNickname().empty())
					{
						std::string message = extractMessage(argument, 2);
						std::string msg = ":" + _current_nick + " PRIVMSG " + singleTarget + " :" + message;
						channel->broadcast(msg, _current_client);
					}
				}
				else
				{
					Client *targetClient = getClientOrError(server, singleTarget);
					if (!targetClient)
						return false; // Error already sent by getClientOrError
					if (!_current_client->getNickname().empty())
					{
						std::string message = extractMessage(argument, 2);
						std::string msg = ":" + _current_nick + " PRIVMSG " + singleTarget + " :" + message;
						targetClient->reply(msg);
					}
				}
				target = target.substr(i + 1);
				i = -1; // reset index for new target
			}
		}

		return false;
	}

	if (target[0] == '#')
	{
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
	else if (target[0] != '#' || target[0] != '&')
	{
		Client *targetClient = getClientOrError(server, target);
		if (!targetClient)
			return false; // Error already sent by getClientOrError
		if (!_current_client->getNickname().empty())
		{
			std::string message = extractMessage(argument, 2);
			std::string msg = ":" + _current_nick + " PRIVMSG " + target + " :" + message;
			targetClient->reply(msg);
		}
		return true;
	}
	else
	{
		_current_client->reply(ERR_NOSUCHNICK(_current_nick, target));
		return false;
	}

	return true;
}
