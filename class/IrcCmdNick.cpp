#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdNick(std::vector<String> argument, Server &server)
{
	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NONICKNAMEGIVEN(_current_nick));
		return false;
	}

	String nick = argument[1];
	if (nick.empty() || nick.length() > 30 || !isalnum(nick[0]))
	{
		_current_client->reply(ERR_ERRONEUSNICKNAME(_current_nick, nick));
		return false;
	}
	if (server.findConnectedByNickname(nick) != NULL)
	{
		_current_client->reply(ERR_NICKNAMEINUSE(_current_nick, nick));
		return false;
	}

	_current_client->setNickname(nick);

	if (!_current_client->getNickname().empty() && !_current_client->getUsername().empty())
	{
		if (!_current_client->isRegistered())
		{
			_current_client->setRegistered(true);
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
