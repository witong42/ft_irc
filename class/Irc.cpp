/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:56 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/07 18:56:56 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include "Irc.hpp"
#include "../header/String.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"



bool Irc::CmdNick(std::vector<String> argument, Server server)

{
	if (argument.size() < 1)
	{
		std::cerr << "Invalid NICK command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	// Handle NICK command
	std::cout << "Handling NICK command: " << argument[0] << server.getfd() << std::endl;
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
	std::cout << "Handling USER command: " <<  argument[0]  << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdJoin(std::vector<String> argument, Server server)
{
	std::cout << "CmdJoin called with argument size: " << argument.size() << " for fd: " << server.getfd() << std::endl;
	
	if (argument.size() < 1)
	{
		std::cerr << "Invalid JOIN command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	// Handle JOIN command
	std::cout << "Handling JOIN command: " <<  argument[0]  << server.getfd() << std::endl;
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
	std::cout << "Handling PART command: " <<  argument[0]  << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdPrivmsg(std::vector<String> vector_buffer,  Server server)
{
	if (vector_buffer.size() < 1)
	{
		std::cerr << "Invalid PRIVMSG command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	// Handle PRIVMSG command
	std::cout << "Handling PRIVMSG command: " <<  vector_buffer[0]  << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdPassw(std::vector<String> argument,  Server server){
	
	
	if (argument.size() < 1)
	{
		std::cerr << "Invalid PASS command format from fd: " << server.getfd() << std::endl;
		return false;
	}
	
		

	if( argument[0] != "")
	{
		if (server.CheckPassword(argument[0]))
		{
			std::cout << "Password accepted for fd: " << server.getfd() << std::endl;
			return true;
		}
		else
		{
			std::cout << "Password rejected for fd: " << server.getfd() << std::endl;
			
		}
		//std::string password =argument[1]; // Extract password after "PASS "
		// Here you would typically check the password against the server's password
		//std::cout << "Received PASS command with password: " << password << " from fd: " << server.getfd() << std::endl;
	}
	
	else
	{
		std::cerr << "Invalid PASS command format from fd: " << server.getfd() << std::endl;
	}
	return false;
}