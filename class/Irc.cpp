/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:56 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/25 21:29:32 by witong           ###   ########.fr       */
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
	commandMap["PASS"] = &Irc::CmdPass;
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

void Irc::disconnectClient(Client *client, std::string reason)
{
	std::string quitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " QUIT :" + reason;
	std::map<String, Channel *>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		Channel *channel = it->second;
		if (channel->hasUser(client))
		{
			channel->broadcast(quitMsg, client);
			channel->removeUser(client);
			if (channel->getUserCount() == 0)
			{
				delete channel;
				_channels.erase(it++);
				continue;
			}
		}
		it++;
	}
}

std::string Irc::extractMessage(const std::vector<String> &argument, size_t start)
{
	std::string message = "";
	if (argument.size() > start)
	{
		message = argument[start];
		for (size_t i = start + 1; i < argument.size(); i++)
			message += " " + argument[i];
		if (!message.empty() && message[0] == ':')
			message = message.substr(1);
	}
	return message;
}

bool Irc::checkRegistered()
{
	if (!_current_client->isRegistered())
	{
		_current_client->reply(ERR_NOTREGISTERED(_current_nick));
		return false;
	}
	return true;
}

bool Irc::checkParams(size_t count, size_t min, std::string cmdName)
{
	if (count < min)
	{
		_current_client->reply(ERR_NEEDMOREPARAMS(_current_nick, cmdName));
		return false;
	}
	return true;
}

Channel *Irc::getChannelOrError(std::string name)
{
	Channel *channel = findChannel(name);
	if (!channel)
	{
		_current_client->reply(ERR_NOSUCHCHANNEL(_current_nick, name));
		return NULL;
	}
	return channel;
}

Client *Irc::getClientOrError(Server &server, std::string nick)
{
	Client *target = server.findConnectedByNickname(nick);
	if (!target)
	{
		_current_client->reply(ERR_NOSUCHNICK(_current_nick, nick));
		return NULL;
	}
	return target;
}
