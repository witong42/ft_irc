/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:52:56 by witong            #+#    #+#             */
/*   Updated: 2026/01/21 18:42:18 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include "../header/Client.hpp"

class Client;

class Channel
{
	private:
		std::string		_name;
		std::string		_topic;

		std::map<Client *, bool>	_users;
		std::vector<Client *>	_invited;

		// MODE
		bool					_isInviteOnly;		// +i
		bool					_isTopicRestricted;	// +t
		std::string				_key;				// +k
		std::string				_limit;				// +l

		// MODE Helpers
		struct ModeContext
		{
			bool adding;
			size_t argIdx;
			const std::vector<std::string> &args;
			std::string appliedArgs;
			std::string appliedModes;
			char lastSign;

			// Constructor
			ModeContext(const std::vector<std::string> &a) : adding(true), argIdx(0), args(a), appliedArgs(""), appliedModes(""), lastSign('\0')
			{
			}
		};
		bool handleModeI(ModeContext &ctx);
		bool handleModeT(ModeContext &ctx);
		bool handleModeK(ModeContext &ctx);
		bool handleModeL(ModeContext &ctx);
		bool handleModeO(ModeContext &ctx);
		void processModeChar(char c, ModeContext &ctx, Client *user);

		void sendChannelModes(Client *user);
		bool checkOperatorPrivileges(Client *user);

	public:
		// Canonical Form
		Channel();
		Channel(const std::string &name, Client *creator);
		Channel(const Channel &other);
		Channel &operator=(const Channel &rhs);
		~Channel();

		// Setters
		void	setTopic(const std::string &topic); // TOPIC
		void	setInviteOnly(bool status); // MODE
		void	setTopicRestricted(bool status); // MODE
		void	setKey(const std::string &key); // MODE
		void	setLimit(const std::string &limit); // MODE

		// Getters
		const std::string	&getName() const;
		const std::string	&getTopic() const;
		const std::string	&getKey() const;
		const std::string	&getLimit() const;
		size_t				getUserCount() const;
		std::string			getUserList() const; // JOIN

		// Mode status
		bool	isOperator(Client *user) const;
		bool	isInviteOnly() const;
		bool	isTopicRestricted() const;
		bool	hasKey() const;
		bool	hasLimit() const;
		bool	hasUser(Client *user) const;

		// Mode operations
		void	addOperator(Client *user); // MODE
		void	removeOperator(Client *user); // MODE

		// Actions
		void	addUser(Client *user); // JOIN
		void	removeUser(Client *user); // PART / QUIT / KICK
		void	invite(Client *user); // INVITE
		void	kick(Client *kicker, const std::string &targetNick, const std::string &reason); // KICK
		void	changeTopic(Client *user, std::string topic); // TOPIC

		Client	*findUserByNickname(const std::string &nickname);

		void	broadcast(const std::string &msg); // PRIVMSG / NOTICE
		void	broadcast(const std::string &msg, Client *excludeUser); // PRIVMSG / NOTICE
		void	mode(Client *user, const std::string &modes, const std::vector<std::string> &args); // MODE
};

#endif
