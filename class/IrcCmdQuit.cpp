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

	if (!checkRegistered())
		return false;

	std::string reason = extractMessage(argument, 1);
	ircDisconnectClient(_current_client, reason);
	return true;
}
