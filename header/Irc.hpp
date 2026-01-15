/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:42 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/15 09:26:31 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "../header/String.hpp"
#include "../header/Server.hpp"
#include "../header/Channel.hpp"
#include <string>

class Irc
{
private:
	std::map<String, Channel *> _channels;
	std::map<int, Client *> _invited;

	Channel *findChannel(String channel);

public:
	Irc();
	~Irc();

	bool CmdNick(std::vector<String> argument, Server server);
	bool CmdUser(std::vector<String> argument, Server server);
	bool CmdJoin(std::vector<String> argument, Server server);
	bool CmdPart(std::vector<String> argument, Server server);
	bool CmdPrivmsg(std::vector<String> argument, Server server);
	bool CmdPassw(std::vector<String> argument, Server server);
	bool CmdMode(std::vector<String> argument, Server server);
	bool CmdCap(std::vector<String> argument, Server server);
	bool parseSwitchCommand(std::string cmd, std::string buffer, Server server);
	bool parseCommand(std::string buffer, Server server);
};
