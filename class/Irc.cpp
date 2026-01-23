/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:56 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/23 12:36:09 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sstream>
#include <vector>
#include <map>
#include "../header/String.hpp"
#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"

Irc::Irc() {}
Irc::~Irc() {}

Channel *Irc::findChannel(String channel)
{
	std::map<String, Channel *>::iterator it = _channels.find(channel);
	return (it != _channels.end()) ? it->second : NULL;
}

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

bool Irc::CmdUser(std::vector<String> argument, Server &server)
{
	(void)server;
	if (argument.size() < 5)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "USER"));
		return false;
	}

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

bool Irc::CmdPart(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "PART"));
		return false;
	}

	std::string channelName = argument[1];
	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		_current_client->reply(ERR_NOSUCHCHANNEL(_current_nick, channelName));
		return false;
	}

	if (!channel->hasUser(_current_client))
	{
		_current_client->reply(ERR_NOTONCHANNEL(_current_nick, channelName));
		return false;
	}
	std::string reason = "";
	if (argument.size() > 2)
	{
		reason = argument[2];
		for (size_t i = 3; i < argument.size(); i++)
			reason += " " + argument[i];
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}

	std::string msg = ":" + _current_nick + "!" + _current_client->getUsername() + "@" + _current_client->getIp() + " PART " + channelName;
	if (!reason.empty())
		msg += " :" + reason;

	channel->broadcast(msg);
	channel->removeUser(_current_client);

	if (channel->getUserCount() == 0)
	{
		_channels.erase(channelName);
		delete channel;
	}
	return true;
}

bool Irc::CmdMode(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "MODE"));
		return false;
	}
	std::string target = argument[1];

	Channel *channel = findChannel(target);
	if (channel)
	{
		std::string modes = "";
		if (argument.size() > 2)
			modes = argument[2];
		std::vector<std::string> modeArgs;
		for (size_t i = 3; i < argument.size(); i++)
		{
			modeArgs.push_back(std::string(argument[i]));
		}
		channel->mode(_current_client, modes, modeArgs);
	}
	return true;
}

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

bool Irc::CmdPassw(std::vector<String> argument, Server &server)
{
	(void)server;
	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "PASS"));
		return false;
	}

	if (_current_client->isRegistered())
	{
		_current_client->reply(ERR_ALREADYREGISTERED(_current_nick));
		return false;
	}

	return true;
}

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
bool Irc::CmdKick(std::vector<String> argument, Server &server)
{
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 3)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "KICK"));
		return false;
	}

	std::string channelName = argument[1];
	std::string targetNick = argument[2];
	std::string reason = _current_nick;

	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		_current_client->reply(ERR_NOSUCHCHANNEL(_current_nick, channelName));
		return false;
	}

	if (!channel->hasUser(_current_client))
	{
		_current_client->reply(ERR_NOTONCHANNEL(_current_nick, channelName));
		return false;
	}

	if (!channel->isOperator(_current_client))
	{
		_current_client->reply(ERR_CHANOPRIVSNEEDED(_current_nick, channelName));
		return false;
	}

	Client *target = server.findConnectedByNickname(targetNick);
	if (!target || !channel->hasUser(target))
	{
		_current_client->reply(ERR_USERNOTINCHANNEL(_current_nick, targetNick, channelName));
		return false;
	}
	channel->kick(_current_client, targetNick, reason);
	return true;
}

bool Irc::CmdInvite(std::vector<String> argument, Server &server)
{
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 3)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "INVITE"));
		return false;
	}

	std::string targetNick = argument[1];
	std::string channelName = argument[2];

	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		_current_client->reply(ERR_NOSUCHCHANNEL(_current_nick, channelName));
		return false;
	}

	Client *target = server.findConnectedByNickname(targetNick);
	if (!target)
	{
		_current_client->reply(ERR_NOSUCHNICK(_current_nick, targetNick));
		return false;
	}

	if (channel->hasUser(target))
	{
		_current_client->reply(ERR_USERONCHANNEL(_current_nick, targetNick, channelName));
		return false;
	}

	channel->invite(target);
	std::string inviteMsg = ":" + _current_nick + " INVITE " + targetNick + " :" + channelName;
	if (target)
		target->reply(inviteMsg);

	_current_client->reply(RPL_INVITING(_current_nick, channelName, targetNick));

	return true;
}

bool Irc::CmdTopic(std::vector<String> argument, Server &server)
{
	(void)server;
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

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

		std::string newTopic = argument[2];
		for (size_t i = 3; i < argument.size(); i++)
			newTopic += " " + argument[i];
		if (!newTopic.empty() && newTopic[0] == ':')
			newTopic = newTopic.substr(1);

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
bool Irc::CmdPing(std::vector<String> argument, Server &server)
{
	std::cout << "Received Ping command" << server.getServerName() << std::endl;

	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "PING"));
		return false;
	}

	std::string server1 = std::string(argument[1]);
	std::string server2 = argument.size() >= 3 ? std::string(argument[2]) : "";

	std::string response = "PONG " + server1;
	if (!server2.empty())
		response += " " + server2;

	_current_client->reply(response);
	return true;
}

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

bool Irc::CmdPong(std::vector<String> argument, Server &server)
{
	std::cout << "Received PONG command" << server.getServerName() << std::endl;

	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	if (argument.size() < 2)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, "PONG"));
		return false;
	}
	return true;
}

bool Irc::CmdQuit(std::vector<String> argument, Server &server)
{
	std::cout << "Received QUIT command" << server.getServerName() << std::endl;

	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}

	std::string reason = "Client Quit";
	if (argument.size() >= 2)
	{
		reason = argument[1];
		for (size_t i = 2; i < argument.size(); i++)
			reason += " " + argument[i];
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}

	std::string quitMsg = ":" + _current_nick + "!" + _current_client->getUsername() + "@" + _current_client->getIp() + " QUIT :" + reason;

	std::map<String, Channel *>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		Channel *channel = it->second;
		if (channel->hasUser(_current_client))
		{
			channel->broadcast(quitMsg, _current_client);
			channel->removeUser(_current_client);
			if (channel->getUserCount() == 0)
			{
				delete channel;
				_channels.erase(it++);
				continue;
			}
		}
		it++;
	}
	return true;
}

bool Irc::parseCommand(std::string buffer, Server &server)
{
	String str(buffer);
	std::vector<String> parts = str.split("\r\n");
	for (size_t i = 0; i < parts.size(); ++i)
	{
		if (parts[i].empty())
			continue;
		std::istringstream lineStream(parts[i]);
		std::string lineCmd;
		std::getline(lineStream, lineCmd, ' ');
		parseSwitchCommand(lineCmd, parts[i], server);
	}
	return true;
}

void Irc::setCurrentClient(Server &server)
{
	this->_current_client = server.findConnectedByfd(server.getClientFd());
	if (this->_current_client && !this->_current_client->getNickname().empty())
		this->_current_nick = this->_current_client->getNickname();
	else
		this->_current_nick = "*";
}

bool Irc::parseSwitchCommand(std::string cmd, std::string buffer, Server &server)
{
	String str(buffer);
	std::vector<String> parts = str.split(" ");
	if (parts.size() == 0)
		return true;
	cmd = parts[0];
	std::map<std::string, bool (Irc::*)(std::vector<String>, Server &)> commandMap;
	commandMap["PASS"] = &Irc::CmdPassw;
	commandMap["NICK"] = &Irc::CmdNick;
	commandMap["USER"] = &Irc::CmdUser;
	commandMap["JOIN"] = &Irc::CmdJoin;
	commandMap["PART"] = &Irc::CmdPart;
	commandMap["MODE"] = &Irc::CmdMode;
	commandMap["CAP"] = &Irc::CmdCap;
	commandMap["KICK"] = &Irc::CmdKick;
	commandMap["INVITE"] = &Irc::CmdInvite;
	commandMap["TOPIC"] = &Irc::CmdTopic;
	commandMap["PRIVMSG"] = &Irc::CmdPrivmsg;
	commandMap["PING"] = &Irc::CmdPing;
	commandMap["PONG"] = &Irc::CmdPong;
	commandMap["WHO"] = &Irc::CmdWho;
	commandMap["QUIT"] = &Irc::CmdQuit;

	if (commandMap.find(cmd) != commandMap.end())
	{
		setCurrentClient(server);
		if (!_current_client)
			return false;
		return (this->*(commandMap[cmd]))(str.get_vector(), server);
	}
	else
	{
		std::cerr << "Command not recognized: " << cmd << std::endl;
	}
	return true;
}
