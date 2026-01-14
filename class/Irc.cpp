/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:56 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/14 16:13:43 by jegirard         ###   ########.fr       */
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
	if (argument.size() < 2) // Adjusted for index 1 being the nick
	{
		std::string error = "431 :No nickname given\r\n";
		send(server.getClientFd(), error.c_str(), error.length(), 0);
		return false;
	}
	// Basic nick validation (add more as needed)
	String nick = argument[1];
	if (nick.empty() || nick.length() > 9 || !isalnum(nick[0]))
	{
		std::string error = "432 " + nick + " :Erroneous nickname\r\n";
		send(server.getClientFd(), error.c_str(), error.length(), 0);
		return false;
	}
	// Set nick in client
	Client *client = server.findInvitedByfd(server.getClientFd());
	if (client)
	{
		client->setNickname(nick);
	}
	std::cout << "Handling NICK command: " << nick << " for fd: " << server.getClientFd() << std::endl;
	// Send RPL_WELCOME if USER was already sent (simplified)
	// For now, just log; full impl needs state tracking
	return true;
}

bool Irc::CmdUser(std::vector<String> argument, Server server)
{
	if (argument.size() < 5) // USER <username> <hostname> <servername> <realname>
	{
		std::string error = "461 USER :Not enough parameters\r\n";
		send(server.getClientFd(), error.c_str(), error.length(), 0);
		return false;
	}
	// Set username in client
	Client *client = server.findInvitedByfd(server.getClientFd());
	if (client)
	{
		client->setUsername(argument[1]);
	}
	std::cout << "Handling USER command: " << argument[1] << " for fd: " << server.getClientFd() << std::endl;
	// Send welcome if NICK was set
	if (client && !client->getNickname().empty())
	{
		std::string welcome = "001 " + client->getNickname() + " :Welcome to the ft_irc server!\r\n";
		welcome += "002 " + client->getNickname() + " :This is a sample message for code 002\r\n";
		welcome += "003 " + client->getNickname() + " :This is a sample message for code 003\r\n";
		send(server.getClientFd(), welcome.c_str(), welcome.length(), 0);
		std::cout << "Sent welcome to fd: " << server.getClientFd() << std::endl;
	}
	return true;
}

Channel *Irc::findChannel(String channel)
{
	std::map<String, Channel *>::iterator it = _channels.find(channel);
	return (it != _channels.end()) ? it->second : NULL;
}

bool Irc::CmdJoin(std::vector<String> argument, Server server)
{
	std::cout << "CmdJoin called with argument size: " << argument.size() << " for fd: " << server.getClientFd() << std::endl;

	if (argument.size() < 2)
	{
		std::cerr << "Invalid JOIN command format from fd: " << server.getClientFd() << std::endl;
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
	std::cout << "Handling JOIN command: " << argument[1] << server.getClientFd() << std::endl;
	return true;
}
bool Irc::CmdPart(std::vector<String> argument, Server server)
{
	if (argument.size() < 1)
	{
		std::cerr << "Invalid PART command format from fd: " << server.getClientFd() << std::endl;
		return false;
	}
	// Handle PART command
	std::cout << "Handling PART command: " << argument[1] << server.getClientFd() << std::endl;
	return true;
}
bool Irc::CmdPrivmsg(std::vector<String> argument, Server server)
{
	if (argument.size() < 3)
	{
		std::cerr << "Invalid PRIVMSG command format from fd: " << server.getClientFd() << std::endl;
		return false;
	}
	// Handle PRIVMSG command
	std::cout << "Handling PRIVMSG command: " << argument[1] << "|" << server.getfd() << std::endl;
	std::cout << "Message content: " << argument[2] << std::endl;
	std::cout << "Broadcasting message to channel: " << argument[1] << std::endl;
	std::cout << "Message: " << "Message to channel " + argument[2] << std::endl;
	std::cout << "Channel exists: " << (this->_channels.find(argument[1]) != this->_channels.end() ? "Yes" : "No") << std::endl;
	std::cout << "number of chennel" << this->_channels.size();
	// this->_channels[vector_buffer[1]]->broadcast("Message to channel " + vector_buffer[2]);
	// send(server.getClientFd(), vector_buffer[2].c_str(), vector_buffer[2].length(), 0);

	if (this->_channels.find(argument[1]) != this->_channels.end())
	{
		Client *sender = server.findInvitedByfd(server.getClientFd());
		if (sender && !sender->getNickname().empty())
		{
			std::string msg = ":" + sender->getNickname() + " PRIVMSG " + argument[1] + " " + argument[2] + "\r\n";
			this->_channels[argument[1]]->broadcast(msg, sender);
		}
	}
	else
	{
		std::string error = "403 " + argument[1] + " :No such channel\r\n";
		send(server.getClientFd(), error.c_str(), error.length(), 0);
	}

	return true;
}

bool Irc::CmdMode(std::vector<String> argument, Server server)
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
bool Irc::CmdPassw(std::vector<String> argument, Server server)
{

	if (argument.size() < 1)
	{
		std::cerr << "Invalid PASS command format from fd: " << server.getClientFd() << std::endl;
		return false;
	}
	std::cout << "CmdPassw called with argument size: " << argument.size() << " for fd: " << server.getClientFd() << std::endl;
	std::cout << "Password attempt: '" << argument[1] << std::endl;
	if (argument[1] != "")
	{
		if (server.CheckPassword(argument[1], server.getClientFd()))
		{
			std::cout << "Password accepted for fd: " << server.getClientFd() << std::endl;
			return true;
		}
		else
		{
			std::cout << "Password rejected for fd: " << server.getClientFd() << std::endl;
		}
		// std::string password =argument[1]; // Extract password after "PASS "
		//  Here you would typically check the password against the server's password
		// std::cout << "Received PASS command with password: " << password << " from fd: " << server.getfd() << std::endl;
	}

	else
	{
		std::cerr << "Invalid PASS command format from fd: " << server.getClientFd() << std::endl;
	}
	return false;
}

bool Irc::CmdCap(std::vector<String> argument, Server server)
{
	if (argument.size() > 1)
	{
		if (argument[1] == "LS")
		{
			std::string response = "CAP * LS :\r\n";
			send(server.getClientFd(), response.c_str(), response.length(), 0);
		}
		else if (argument[1] == "REQ")
		{
			std::string response = "CAP * NAK :" + (argument.size() > 2 ? argument[2] : String("")) + "\r\n";
			send(server.getClientFd(), response.c_str(), response.length(), 0);
		}
		else if (argument[1] == "END")
		{
			// Terminate CAP negotiation
			std::string response = "CAP * ACK :END\r\n";
			send(server.getClientFd(), response.c_str(), response.length(), 0);
		}
	}
	return true;
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
	// send(_fd_client, buffer, count, 0);
	std::cout << "parseCommand  fd client" << server.getClientFd();

	//send(server.getClientFd(), buffer.c_str(), buffer.length(), 0);
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
	commandMap["MODE"] = &Irc::CmdMode;
	commandMap["CAP"] = &Irc::CmdCap;
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