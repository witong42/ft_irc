/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:42 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/22 10:20:22 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "../header/String.hpp"
#include "../header/Server.hpp"
#include "../header/Channel.hpp"
#include "../header/Client.hpp"
#include <map>
#
#include <string>

class Irc
{
private:
	std::map<String, Channel *> _channels;
	std::map<int, Client *> _clients;
	Client *_curent_client;
	std::string _curent_nick; 
	

	Channel *findChannel(String channel);
	bool CmdNick(std::vector<String> argument, Server &server);
	bool CmdUser(std::vector<String> argument, Server &server);
	bool CmdJoin(std::vector<String> argument, Server &server);
	bool CmdPart(std::vector<String> argument, Server &server);
	bool CmdMode(std::vector<String> argument, Server &server);
	bool CmdPrivmsg(std::vector<String> argument, Server &server);
	bool CmdPassw(std::vector<String> argument, Server &server);
	bool CmdCap(std::vector<String> argument, Server &server);
	bool CmdKick(std::vector<String> argument, Server &server);
	bool CmdInvite(std::vector<String> argument, Server &server);
	bool CmdTopic(std::vector<String> argument, Server &server);
	bool parseSwitchCommand(std::string cmd, std::string buffer, Server &server);
	void setCurrentClient(Server &server);

public:
	Irc();
	~Irc();
	bool parseCommand(std::string buffer, Server &server);
};
