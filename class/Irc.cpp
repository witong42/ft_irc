/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:56 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/23 11:36:29 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Irc.hpp"


bool Irc::CmdNick(std::vector<String> vector_buffer, Server server)
{
	// Handle NICK command
	std::cout << "Handling NICK command: " << vector_buffer[1] << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdUser(std::vector<String> vector_buffer, Server server)
{
	// Handle USER command
	std::cout << "Handling USER command: " <<  vector_buffer[1]  << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdJoin(std::vector<String> vector_buffer, Server server)
{
	// Handle JOIN command
	std::cout << "Handling JOIN command: " <<  vector_buffer[1]  << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdPart(std::vector<String> vector_buffer, Server server)
{
	// Handle PART command
	std::cout << "Handling PART command: " <<  vector_buffer[1]  << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdPrivmsg(std::vector<String> vector_buffer,  Server server)
{
	// Handle PRIVMSG command
	std::cout << "Handling PRIVMSG command: " <<  vector_buffer[1]  << server.getfd() << std::endl;
	return true;
}
bool Irc::CmdPassw(std::vector<String> vector_buffer,  Server server){

	if( vector_buffer[1] != "")
	{
		if (server.checkPassword(vector_buffer[1]))
		{
			std::cout << "Password accepted for fd: " << server.getfd() << std::endl;
			return true;
		}
		else
		{
			std::cout << "Password rejected for fd: " << server.getfd() << std::endl;
			
		}
		std::string password =vector_buffer[1]; // Extract password after "PASS "
		// Here you would typically check the password against the server's password
		std::cout << "Received PASS command with password: " << password << " from fd: " << server.getfd() << std::endl;
	}
	
	else
	{
		std::cerr << "Invalid PASS command format from fd: " << server.getfd() << std::endl;
	}
	return false;
}