#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::CmdTopic(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!checkRegistered())
		return false;

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "TOPIC"));
		return false;
	}

	std::string channelName = argument[1];
	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		_current_client->reply(ERR_NOSUCHCHANNEL(_current_nick, channelName));
		return false;
	}

	if (argument.size() >= 3)
	{
		if (channel->isTopicRestricted() && !channel->isOperator(_current_client))
		{
			_current_client->reply(ERR_CHANOPRIVSNEEDED(_current_nick, channelName));
			return false;
		}

		std::string newTopic = extractMessage(argument, 2);

		channel->setTopic(newTopic);

		std::string msg = ":" + _current_nick + " TOPIC " + channelName + " :" + newTopic;
		channel->broadcast(msg, NULL);
	}
	else
	{
		std::string topic = channel->getTopic();
		if (topic.empty())
		{
			_current_client->reply(RPL_NOTOPIC(_current_nick, channelName));
		}
		else
		{
			_current_client->reply(RPL_TOPIC(_current_nick, channelName, topic));
		}
	}
	return true;
}
