#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdCap(std::vector<String> argument, Server &server)
{
	(void)server;
	if (argument.size() > 1)
	{
		if (argument[1] == "LS")
		{
			std::string response = "CAP * LS :";
			_current_client->reply(response);
		}
		else if (argument[1] == "END")
		{
			std::string response = "CAP * ACK :END";
			_current_client->reply(response);
		}
	}
	return true;
}
