#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdWho(std::vector<String> argument, Server &server)
{
	if (!checkRegistered())
		return false;
	if (!checkParams(argument.size(), 2, "WHO"))
		return false;
	std::string target = argument[1];
	Client *targetClient = server.findConnectedByNickname(target);
	if (targetClient)
	{
		std::string response = RPL_WHOREPLY(targetClient->getNickname(), targetClient->getUsername(), targetClient->getIp(), server.getServerName(), this->_current_nick, "H", "0");
		_current_client->reply(response);
		_current_client->reply(RPL_ENDOFWHO(_current_nick, target));
		return true;
	}
	std::cout << "Processing WHO command for target: " << target << std::endl;
	if (target.empty())
	{
		_current_client->reply(RPL_ENDOFWHO(_current_nick, target));
		return true;
	}
	if (target[1] == '*')
	{
		// List all connected clients
		for (std::map<int, Client *>::iterator it = server.getConnectedClients().begin(); it != server.getConnectedClients().end(); ++it)
		{
			std::cout << "Processing WHO for client fd: " << it->first << std::endl;
			Client *client = it->second;
			std::string response = RPL_WHOREPLY(client->getNickname(), client->getUsername(), client->getIp(), server.getServerName(), client->getNickname(), "H", "0");
			_current_client->reply(response);
		}
		_current_client->reply(RPL_ENDOFWHO(_current_nick, target));
		return true;
	}
	if (argument[1][0] == '#')
	{
		// Target is a channel
		Channel *channel = findChannel(argument[1]);
		if (!channel)
		{
			_current_client->reply(RPL_ENDOFWHO(_current_nick, target));
			return true;
		}
		std::map<Client *, bool> users = channel->getUsers();
		for (std::map<Client *, bool>::iterator it = users.begin(); it != users.end(); ++it)
		{
			Client *client = it->first;
			std::string response = RPL_WHOREPLY(client->getNickname(), client->getUsername(), client->getIp(), server.getServerName(), client->getNickname(), "H", "0");
			_current_client->reply(response);
		}
		_current_client->reply(RPL_ENDOFWHO(_current_nick, target));
		return true;
	}
	_current_client->reply(RPL_ENDOFWHO(_current_nick, target));
	return true;
}
