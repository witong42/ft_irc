/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:42 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/29 10:29:56 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
#define IRC_HPP

#include <vector>
#include "../header/String.hpp"
#include "../header/Server.hpp"
#include "../header/Channel.hpp"
#include "../header/Client.hpp"
#include <map>
#include <string>

class Irc
{
private:
	std::map<String, Channel *> _channels;
	Client *_current_client;
	std::string _current_nick;

	Channel *findChannel(String channel);
	bool CmdNick(std::vector<String> argument, Server &server);
	bool CmdUser(std::vector<String> argument, Server &server);
	bool CmdJoin(std::vector<String> argument, Server &server);
	bool CmdPart(std::vector<String> argument, Server &server);
	bool CmdMode(std::vector<String> argument, Server &server);
	bool CmdPrivmsg(std::vector<String> argument, Server &server);
	bool CmdPass(std::vector<String> argument, Server &server);
	bool CmdCap(std::vector<String> argument, Server &server);
	bool CmdKick(std::vector<String> argument, Server &server);
	bool CmdInvite(std::vector<String> argument, Server &server);
	bool CmdTopic(std::vector<String> argument, Server &server);
	bool CmdPing(std::vector<String> argument, Server &server);
	bool CmdWho(std::vector<String> argument, Server &server);
	bool CmdPong(std::vector<String> argument, Server &server);
	bool CmdQuit(std::vector<String> argument, Server &server);
	bool TryRegisterClient(Server &server);
	void setCurrentClient(Server &server);

public:
	Irc();
	~Irc();
	bool switchCommand(std::string buffer, Server &server);
	// bool parseCommand(std::string buffer, Server &server);

	// Actions
	void ircDisconnectClient(Client *client, std::string reason);
	void removeChannel(std::string name);

	// Checks & Getters
	bool checkRegistered();
	bool checkParams(size_t count, size_t min, std::string cmdName);
	Channel *getChannelOrError(std::string name);
	Client *getClientOrError(Server &server, std::string nick);

	// Utils
	std::string extractMessage(const std::vector<String> &argument, size_t start);
};

#endif
