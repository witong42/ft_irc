/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:56 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/13 17:26:53 by jegirard         ###   ########.fr       */
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
#include "String.hpp"

#include "Irc.hpp"
#include "../header/String.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"

Irc::Irc() {}
Irc::~Irc() {}

bool Irc::CmdNick(std::vector<String> argument, Server server)

{
	if (argument.size() < 1)
	{
		std::cerr << "Invalid NICK command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	// Handle NICK command
	std::cout << "Handling NICK command: " << argument[1] << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdUser(std::vector<String> argument, Server server)
{
	if (argument.size() < 1)
	{
		std::cerr << "Invalid USER command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	// Handle USER command
	std::cout << "Handling USER command: " << argument[1] << server.getfd() << std::endl;
	return true;
}

Channel *Irc::findChannel(String channel)
{
	std::map<String, Channel *>::iterator it = _channels.find(channel);
	return (it != _channels.end()) ? it->second : NULL;
}

bool Irc::CmdJoin(std::vector<String> argument, Server server)
{
	std::cout << "CmdJoin called with argument size: " << argument.size() << " for fd: " << server.getfd() << std::endl;

	if (argument.size() < 1)
	{
		std::cerr << "Invalid JOIN command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	// Channel* channel = findChannel(argument[0]);

	// std::map<int, Client *>::iterator it = this->_invited.find(server.getfd());
	std::map<String, Channel *>::iterator it = _channels.find(argument[1]);
	Client *invitedUser = server.findInvitedByfd(server.getClientFd());
	if (it != this->_channels.end())
	{
		std::cout << "Channel " << argument[1] << " found for JOIN command." << std::endl;
		it->second->addUser(invitedUser);
	}
	else
	{
		std::cout << "Channel " << argument[1] << " not found. Creating new channel." << std::endl;
		Channel *newChannel = new Channel(argument[1], NULL);
		
		std::cout << "Created new channel: fd " << server.getClientFd() << std::endl;
		std::cout << "Added user fd " << invitedUser->getFd() << std::endl;
		newChannel->addUser(invitedUser);
		newChannel->addOperator(invitedUser);
		// Assuming NULL for creator for now
		this->_channels[argument[1]] = newChannel;
	}

	// Handle JOIN command
	std::cout << "Handling JOIN command: " << argument[1] << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdPart(std::vector<String> argument, Server server)
{
	if (argument.size() < 1)
	{
		std::cerr << "Invalid PART command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	// Handle PART command
	std::cout << "Handling PART command: " << argument[1] << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdPrivmsg(std::vector<String> vector_buffer, Server server)
{
	if (vector_buffer.size() < 1)
	{
		std::cerr << "Invalid PRIVMSG command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	// Handle PRIVMSG command
	std::cout << "Handling PRIVMSG command: " << vector_buffer[1] << "|" << server.getfd() << std::endl;
	std::cout << "Message content: " << vector_buffer[2] << std::endl;
	std::cout << "Broadcasting message to channel: " << vector_buffer[1] << std::endl;
	std::cout << "Message: " << "Message to channel " + vector_buffer[2] << std::endl;
	std::cout << "Channel exists: " << (this->_channels.find(vector_buffer[1]) != this->_channels.end() ? "Yes" : "No") << std::endl;
	std::cout << "number of chennel" << this->_channels.size();
	this->_channels[vector_buffer[1]]->broadcast("Message to channel " + vector_buffer[2]);
	//send(server.getClientFd(), vector_buffer[2].c_str(), vector_buffer[2].length(), 0);
	return true;
}
bool Irc::CmdPassw(std::vector<String> argument, Server server)
{

	if (argument.size() < 1)
	{
		std::cerr << "Invalid PASS command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	std::cout << "CmdPassw called with argument size: " << argument.size() << " for fd: " << server.getfd() << std::endl;
	std::cout << "Password attempt: '" << argument[1] << std::endl;
	if (argument[1] != "")
	{
		if (server.CheckPassword(argument[1], server.getfd()))
		{
			std::cout << "Password accepted for fd: " << server.getfd() << std::endl;
			return true;
		}
		else
		{
			std::cout << "Password rejected for fd: " << server.getfd() << std::endl;
		}
		// std::string password =argument[1]; // Extract password after "PASS "
		//  Here you would typically check the password against the server's password
		// std::cout << "Received PASS command with password: " << password << " from fd: " << server.getfd() << std::endl;
	}

	else
	{
		std::cerr << "Invalid PASS command format from fd: " << server.getfd() << std::endl;
	}
	return false;
}

bool Irc::parseCommand(std::string buffer, Server server)
{

	// Command parsing implementation

	String str(buffer);
	std::cout << "parseCommand buffer: '\n"
			  << buffer << "\n' fd: " << server.getClientFd() << std::endl;
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

	// Echo - renvoyer les données au client
	 //send(_fd_client, buffer, count, 0);
	std::cout << "parseCommand  fd client" << server.getClientFd();
	
	send(server.getClientFd(), buffer.c_str(), buffer.length(), 0);
	return true;
}

bool Irc::parseSwitchCommand(std::string cmd, std::string buffer, Server server)
{
	std::cout << "parseSwitchCommand cmd: '" << cmd << "' buffer: '" << buffer << "'\n fd client: " << server.getClientFd() << std::endl;
	String str(buffer);
	std::vector<String> parts = str.split(" ");
	if (parts.size() == 0)
		return true;
	cmd = parts[0];
	std::map<std::string, bool (Irc::*)(std::vector<String>, Server)> commandMap;
	commandMap["PASS"] = &Irc::CmdPassw;
	commandMap["NICK"] = &Irc::CmdNick;
	commandMap["USER"] = &Irc::CmdUser;
	commandMap["JOIN"] = &Irc::CmdJoin;
	commandMap["PART"] = &Irc::CmdPart;
	commandMap["PRIVMSG"] = &Irc::CmdPrivmsg;
	for (size_t i = 0; i < str.get_vector().size(); ++i)
	{
		std::cout << "Argument " << i << ": '" << str.get_vector()[i] << "'" << std::endl;
	}
	if (commandMap.find(cmd) != commandMap.end())
	{
		// str.pop_front();
		return (this->*(commandMap[cmd]))(str.get_vector(), server); // Notez les parenthèses !
	}
	else
	{
		std::cerr << "Commande non reconnue: " << cmd << std::endl;
	}
	return true;
}