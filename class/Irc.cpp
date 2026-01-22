/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:56 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/22 11:23:27 by jegirard         ###   ########.fr       */
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
	Client *client = server.findConnectedByfd(server.getClientFd());
	std::string currentNick = client ? client->getNickname() : "*";
	if (currentNick.empty()) currentNick = "*";

	if (argument.size() < 2)
	{
		if (client)
			client->reply(ERR_NONICKNAMEGIVEN(currentNick));
		return false;
	}

	String nick = argument[1];
	if (nick.empty() || nick.length() > 9 || !isalnum(nick[0]))
	{
		client->reply(ERR_ERRONEUSNICKNAME(currentNick, nick));
		return false;
	}
	// Si l'utilisateur existe deja
	else if (server.findConnectedByNickname(nick) != NULL)
	{
		client->reply(ERR_NICKNAMEINUSE(currentNick, nick));
		return false;
	}
  
	if (client)
	{
		client->setNickname(nick);
	}

	if (client && !client->getNickname().empty() && !client->getUsername().empty())
	{
		if (!client->isRegistered()) {
			client->setRegistered(true);
			std::string user = client->getUsername();
			std::string host = client->getIp();

			client->reply(RPL_WELCOME(nick, user, host));
			client->reply(RPL_YOURHOST(nick));
			client->reply(RPL_CREATED(nick));
			client->reply(RPL_MYINFO(nick));
		}
	}
	return true;
}

bool Irc::CmdUser(std::vector<String> argument, Server &server)
{
	Client *client = server.findConnectedByfd(server.getClientFd());
	std::string nick = client ? client->getNickname() : "*";
	if (nick.empty()) nick = "*";

	if (argument.size() < 5)
	{
		if (client) client->reply(ERR_NEEDMOREPARAMS(nick, "USER"));
		return false;
	}

	if (client)
	{
		if (client->isRegistered())
		{
			client->reply(ERR_ALREADYREGISTERED(nick));
			return true;
		}
		client->setUsername(argument[1]);
	}

	if (client && !client->getNickname().empty() && !client->getUsername().empty())
	{
		if (!client->isRegistered()) {
			client->setRegistered(true);
			nick = client->getNickname();
			std::string user = client->getUsername();
			std::string host = client->getIp();

			client->reply(RPL_WELCOME(nick, user, host));
			client->reply(RPL_YOURHOST(nick));
			client->reply(RPL_CREATED(nick));
			client->reply(RPL_MYINFO(nick));
		}
	}
	return true;
}

bool Irc::CmdJoin(std::vector<String> argument, Server &server)
{
	Client *client = server.findConnectedByfd(server.getClientFd());
	std::string nick = client ? client->getNickname() : "*";

	if (client && !client->isRegistered())
	{
		client->reply(ERR_NOTREGISTERED(nick));
		return false;
	}

	if (argument.size() < 2)
	{
		if (client) client->reply(ERR_NEEDMOREPARAMS(nick, "JOIN"));
		return false;
	}
	while (!argument[1].empty() && !isalnum(argument[1][argument[1].size() - 1]) && argument[1][argument[1].size() - 1] != '#')
	{
		argument[1] = argument[1].substr(0, argument[1].size() - 1);
	}

	std::map<String, Channel *>::iterator it = _channels.find(argument[1]);
	if (it != this->_channels.end())
	{
		it->second->addUser(client);
	}
	else
	{
		Channel *newChannel = new Channel(argument[1], NULL);
		newChannel->addUser(client);
		newChannel->addOperator(client);
		this->_channels[argument[1]] = newChannel;
	}

	Channel *channel = this->_channels[argument[1]];

	std::string joinMsg = ":" + nick + "!" + client->getUsername() + "@" + client->getIp() + " JOIN :" + argument[1];
	channel->broadcast(joinMsg);

	std::string topic = channel->getTopic();
	if (!topic.empty())
		client->reply(RPL_TOPIC(nick, argument[1], topic));
	else
		client->reply(RPL_NOTOPIC(nick, argument[1]));

	std::string userList = channel->getUserList();
	client->reply(RPL_NAMREPLY(nick, argument[1], userList));
	client->reply(RPL_ENDOFNAMES(nick, argument[1]));

	return true;
}

bool Irc::CmdPart(std::vector<String> argument, Server &server)
{
	Client *client = server.findConnectedByfd(server.getClientFd());
	std::string nick = client ? client->getNickname() : "*";

	if (client && !client->isRegistered())
	{
		client->reply(ERR_NOTREGISTERED(nick));
		return false;
	}

	if (argument.size() < 2)
	{
		if (client) client->reply(ERR_NEEDMOREPARAMS(nick, "PART"));
		return false;
	}

	std::string channelName = argument[1];
	Channel *channel = findChannel(channelName);

	if (!channel)
	{
		if (client) client->reply(ERR_NOSUCHCHANNEL(nick, channelName));
		return false;
	}

	if (!channel->hasUser(client))
	{
		if (client) client->reply(ERR_NOTONCHANNEL(nick, channelName));
		return false;
	}
	std::string reason = "";
	if (argument.size() > 2)
	{
		reason = argument[2];
		for (size_t i = 3; i < argument.size(); i++)
			reason += " " + argument[i];
		if (!reason.empty() && reason[0] == ':') reason = reason.substr(1);
	}

	std::string msg = ":" + nick + "!" + client->getUsername() + "@" + client->getIp() + " PART " + channelName;
	if (!reason.empty()) msg += " :" + reason;

	channel->broadcast(msg);
	channel->removeUser(client);

	if (channel->getUserCount() == 0)
	{
		_channels.erase(channelName);
		delete channel;
	}
	return true;
}

bool Irc::CmdMode(std::vector<String> argument, Server &server)
{
	Client *client = server.findConnectedByfd(server.getClientFd());
	std::string nick = client ? client->getNickname() : "*";

	if (client && !client->isRegistered())
	{
		client->reply(ERR_NOTREGISTERED(nick));
		return false;
	}

	if (argument.size() < 2)
	{
		if (client) client->reply(ERR_NEEDMOREPARAMS(nick, "MODE"));
		return false;
	}
	std::string target = argument[1];
	if (!client)
		return false;

	if (target[0] == '#' || target[0] == '&')
	{
		Channel *channel = findChannel(target);
		if (!channel)
		{
			client->reply(ERR_NOSUCHCHANNEL(nick, target));
			return false;
		}
		std::string modes = (argument.size() > 2) ? std::string(argument[2]) : "";
		std::vector<std::string> modeArgs;
		for (size_t i = 3; i < argument.size(); i++)
		{
			modeArgs.push_back(std::string(argument[i]));
		}
		channel->mode(client, modes, modeArgs);
	}
	return true;
}

bool Irc::CmdPrivmsg(std::vector<String> argument, Server &server)
{
	Client *client = server.findConnectedByfd(server.getClientFd());
	std::string nick = client ? client->getNickname() : "*";

	if (client && !client->isRegistered())
	{
		client->reply(ERR_NOTREGISTERED(nick));
		return false;
	}

	if (argument.size() < 2)
	{
		if (client) client->reply(ERR_NORECIPIENT(nick, "PRIVMSG"));
		return false;
	}
	if (argument.size() < 3)
	{
		if (client) client->reply(ERR_NOTEXTTOSEND(nick));
		return false;
	}

	std::string message = argument[2];
	for (size_t i = 3; i < argument.size(); i++) message += " " + argument[i];

	if (this->_channels.find(argument[1]) != this->_channels.end())
	{
		if (client && !client->getNickname().empty())
		{
			std::string msg = ":" + client->getNickname() + " PRIVMSG " + argument[1] + " " + message;
			this->_channels[argument[1]]->broadcast(msg, client);
		}
	}
	else
	{
		if (client)
			client->reply(ERR_NOSUCHCHANNEL(nick, argument[1]));
	}
	return true;
}

bool Irc::CmdPassw(std::vector<String> argument, Server &server)
{
	Client *client = server.findConnectedByfd(server.getClientFd());
	std::string nick = (client && !client->getNickname().empty()) ? client->getNickname() : "*";

	if (argument.size() < 2)
	{
		if (client) client->reply(ERR_NEEDMOREPARAMS(nick, "PASS"));
		return false;
	}

	if (client && client->isRegistered())
	{
		client->reply(ERR_ALREADYREGISTERED(nick));
		return false;
	}

	if (server.CheckPassword(argument[1], server.getClientFd()))
	{
		return true;
	}
	else
	{
		if (client) client->reply(ERR_PASSWDMISMATCH(nick));
		return false;
	}
}

bool Irc::CmdCap(std::vector<String> argument, Server &server)
{
	Client *client = server.findConnectedByfd(server.getClientFd());

	if (argument.size() > 1 && client)
	{
		if (argument[1] == "LS")
		{
			std::string response = "CAP * LS :";
			client->reply(response);
		}
		else if (argument[1] == "REQ")
		{
			std::string response = "CAP * NAK :" + (argument.size() > 2 ? argument[2] : String(""));
			client->reply(response);
		}
		else if (argument[1] == "END")
		{
			std::string response = "CAP * ACK :END";
			client->reply(response);
		}
	}
	return true;
}
bool Irc::CmdKick(std::vector<String> argument, Server &server)
{
	Client *client = server.findConnectedByfd(server.getClientFd());
	std::string nick = client ? client->getNickname() : "*";

	if (client && !client->isRegistered())
	{
		client->reply(ERR_NOTREGISTERED(nick));
		return false;
	}

	if (argument.size() < 3)
	{
		if (client) client->reply(ERR_NEEDMOREPARAMS(nick, "KICK"));
		return false;
	}

	std::string channelName = argument[1];
	std::string targetNick = argument[2];
	std::string reason = nick;

	if (argument.size() > 3)
	{
		reason = argument[3];
		for (size_t i = 4; i < argument.size(); i++)
			reason += " " + argument[i];
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}

	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		if (client)
			client->reply(ERR_NOSUCHCHANNEL(nick, channelName));
		return false;
	}

	if (!channel->hasUser(client))
	{
		if (client)
			client->reply(ERR_NOTONCHANNEL(nick, channelName));
		return false;
	}

	if (!channel->isOperator(client))
	{
		if (client)
			client->reply(ERR_CHANOPRIVSNEEDED(nick, channelName));
		return false;
	}

	Client *target = server.findConnectedByNickname(targetNick);
	if (!target || !channel->hasUser(target))
	{
		if (client)
			client->reply(ERR_USERNOTINCHANNEL(nick, targetNick, channelName));
		return false;
	}
	channel->kick(client, targetNick, reason);
	return true;
}

bool Irc::CmdInvite(std::vector<String> argument, Server &server)
{
	Client *inviter = server.findConnectedByfd(server.getClientFd());
	std::string inviterNick = (inviter && !inviter->getNickname().empty()) ? inviter->getNickname() : "*";

	if (inviter && !inviter->isRegistered())
	{
		inviter->reply(ERR_NOTREGISTERED(inviterNick));
		return false;
	}

	if (argument.size() < 3)
	{
		if (inviter) inviter->reply(ERR_NEEDMOREPARAMS(inviterNick, "INVITE"));
		return false;
	}

	std::string targetNick = argument[1];
	std::string channelName = argument[2];

	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		if (inviter) inviter->reply(ERR_NOSUCHCHANNEL(inviterNick, channelName));
		return false;
	}

	if (!channel->hasUser(inviter))
	{
		if (inviter) inviter->reply(ERR_NOTONCHANNEL(inviterNick, channelName));
		return false;
	}

	Client *target = server.findConnectedByNickname(targetNick);
	if (!target)
	{
		if (inviter) inviter->reply(ERR_NOSUCHNICK(inviterNick, targetNick));
		return false;
	}

	if (channel->hasUser(target))
	{
		if (inviter) inviter->reply(ERR_USERONCHANNEL(inviterNick, targetNick, channelName));
		return false;
	}

	channel->invite(target);
	std::string inviteMsg = ":" + inviterNick + " INVITE " + targetNick + " :" + channelName;
	if (target)
		target->reply(inviteMsg);

	if (inviter)
		inviter->reply(RPL_INVITING(inviterNick, channelName, targetNick));

	return true;
}

bool Irc::CmdTopic(std::vector<String> argument, Server &server)
{
	Client *client = server.findConnectedByfd(server.getClientFd());
	std::string nick = client ? client->getNickname() : "*";

	if (client && !client->isRegistered())
	{
		client->reply(ERR_NOTREGISTERED(nick));
		return false;
	}

	if (argument.size() < 2)
	{
		if (client) client->reply(ERR_NEEDMOREPARAMS(nick, "TOPIC"));
		return false;
	}

	std::string channelName = argument[1];
	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		if (client)
			client->reply(ERR_NOSUCHCHANNEL(nick, channelName));
		return false;
	}

	if (argument.size() >= 3)
	{
		if (!channel->hasUser(client))
		{
			if (client)
				client->reply(ERR_NOTONCHANNEL(nick, channelName));
			return false;
		}

		if (channel->isTopicRestricted() && !channel->isOperator(client))
		{
			if (client)
				client->reply(ERR_CHANOPRIVSNEEDED(nick, channelName));
			return false;
		}

		std::string newTopic = argument[2];
		for (size_t i = 3; i < argument.size(); i++)
			newTopic += " " + argument[i];
		if (!newTopic.empty() && newTopic[0] == ':')
			newTopic = newTopic.substr(1);

		channel->setTopic(newTopic);

		std::string msg = ":" + nick + " TOPIC " + channelName + " :" + newTopic;
		channel->broadcast(msg, NULL);
	}
	else
	{
		std::string topic = channel->getTopic();
		if (topic.empty())
		{
			if (client) client->reply(RPL_NOTOPIC(nick, channelName));
		}
		else
		{
			if (client) client->reply(RPL_TOPIC(nick, channelName, topic));
		}
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
	this->_curent_client = server.findConnectedByfd(server.getClientFd());
	this->_curent_nick = this->_curent_client ? this->_curent_client->getNickname() : "*";
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
	if (commandMap.find(cmd) != commandMap.end())
	{
    setCurrentClient(server);
		return (this->*(commandMap[cmd]))(str.get_vector(), server);
	}
	else
	{
		std::cerr << "Command not recognized: " << cmd << std::endl;
	}
	return true;
}
