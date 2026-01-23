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
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "PRIVMSG"));
		return false;
	}
	String message = String(argument);
	std::vector<String> header = message.pop_front(2);
	if (this->_channels.find(header[1]) != this->_channels.end())
	{
		if (!_current_client->getNickname().empty())
		{
			std::string msg = ":" + _current_nick + " PRIVMSG " + header[1] + " " + message.join();
			this->_channels[header[1]]->broadcast(msg, _current_client);
		}
	}
	else
	{
		_current_client->reply(ERR_NOSUCHCHANNEL(_current_nick, header[1]));
	}
	return true;
}
