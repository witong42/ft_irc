#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdPass(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!checkParams(argument.size(), 2, "PASS"))
		return false;
	if (_current_client->isRegistered())
	{
		_current_client->reply(ERR_ALREADYREGISTERED(_current_nick));
		return false;
	}
	if (server.CheckPassword(argument[1], _current_client->getFd()))
	{
		_current_client->reply("001 " + _current_nick + " :Welcome to the IRC Network\r\n");
		_current_client->setRegistered(true);
	}
	else
	{
		_current_client->reply("464 " + _current_nick + " :Password incorrect\r\n");
		return false;
	}
	return true;
}
