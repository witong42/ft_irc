#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdUser(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!checkParams(argument.size(), 5, "USER"))
		return false;

	if (_current_client->isRegistered())
	{
		_current_client->reply(ERR_ALREADYREGISTERED(_current_nick));
		return true;
	}
	_current_client->setUsername(argument[1]);

	if (!_current_client->getNickname().empty() && !_current_client->getUsername().empty())
	{
		if (!_current_client->isRegistered())
		{
			_current_client->setRegistered(true);
			std::string nick = _current_client->getNickname();
			std::string user = _current_client->getUsername();
			std::string host = _current_client->getIp();

			_current_client->reply(RPL_WELCOME(nick, user, host));
			_current_client->reply(RPL_YOURHOST(nick));
			_current_client->reply(RPL_CREATED(nick));
			_current_client->reply(RPL_MYINFO(nick));
		}
	}
	return true;
}
