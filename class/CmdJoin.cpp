#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>
#include <map>

bool Irc::CmdJoin(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "JOIN"));
		return false;
	}

	std::string channelName = argument[1];

	std::map<String, Channel *>::iterator it = _channels.find(channelName);
	if (it != this->_channels.end())
	{
		it->second->addUser(_current_client);
	}
	else
	{
		Channel *newChannel = new Channel(channelName, NULL);
		newChannel->addUser(_current_client);
		newChannel->addOperator(_current_client);
		this->_channels[channelName] = newChannel;
	}

	Channel *channel = this->_channels[channelName];

	std::string joinMsg = ":" + _current_nick + "!" + _current_client->getUsername() + "@" + _current_client->getIp() + " JOIN :" + channelName;
	channel->broadcast(joinMsg);

	std::string topic = channel->getTopic();
	if (!topic.empty())
		_current_client->reply(RPL_TOPIC(_current_nick, channelName, topic));
	else
		_current_client->reply(RPL_NOTOPIC(_current_nick, channelName));

	std::string userList = channel->getUserList();
	_current_client->reply(RPL_NAMREPLY(_current_nick, channelName, userList));
	_current_client->reply(RPL_ENDOFNAMES(_current_nick, channelName));

	return true;
}
