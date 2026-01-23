#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdWho(std::vector<String> argument, Server &server)
{
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "WHO"));
		return false;
	}

	std::string target = argument[1];
	Client *targetClient = server.findConnectedByNickname(target);
	if (targetClient)
	{
		std::string response = RPL_WHOREPLY(_current_nick, targetClient->getUsername(), targetClient->getIp(), server.getServerName(), this->_current_nick, "H", "0");
		_current_client->reply(response);
	}
	_current_client->reply(RPL_ENDOFWHO(_current_nick, target));
	return true;
}
