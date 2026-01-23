#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>
#include <map>

bool Irc::CmdQuit(std::vector<String> argument, Server &server)
{
	std::cout << "Received QUIT command" << server.getServerName() << std::endl;

	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	std::string reason = "Client Quit";
	if (argument.size() >= 2)
	{
		reason = argument[1];
		for (size_t i = 2; i < argument.size(); i++)
			reason += " " + argument[i];
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}

	std::string quitMsg = ":" + _current_nick + "!" + _current_client->getUsername() + "@" + _current_client->getIp() + " QUIT :" + reason;

	std::map<String, Channel *>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		Channel *channel = it->second;
		if (channel->hasUser(_current_client))
		{
			channel->broadcast(quitMsg, _current_client);
			channel->removeUser(_current_client);
			if (channel->getUserCount() == 0)
			{
				delete channel;
				_channels.erase(it++);
				continue;
			}
		}
		it++;
	}
	return true;
}
