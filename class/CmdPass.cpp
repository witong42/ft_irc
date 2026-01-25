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

	return true;
}
