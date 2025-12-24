/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:52:56 by witong            #+#    #+#             */
/*   Updated: 2025/12/24 10:42:42 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
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
		size_t					_limit;				// +l
		bool					_hasLimit;			// +l

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
		void	setLimit(size_t limit); // MODE
		void	removeLimit(); // MODE

		// Getters
		const std::string	&getName() const;
		const std::string	&getTopic() const;
		const std::string	&getKey() const;
		size_t				getLimit() const;
		size_t				getUserCount() const;
		std::string			getUserList() const; // JOIN

		// Mode status
		bool	isOperator(Client *user) const;
		bool	isInviteOnly() const;
		bool	isTopicRestricted() const;
		bool	hasLimit() const;
		bool	hasKey() const;
		bool	hasUser(Client *user) const;

		// Mode operations
		void	addOperator(Client *user); // MODE
		void	removeOperator(Client *user); // MODE

		// Actions
		void	join(Client *user); // JOIN
		void	removeUser(Client *user); // PART / QUIT / KICK
		void	kick(Client* kicker, Client *user); // KICK
		void	invite(Client *user); // INVITE
		void	changeTopic(Client *user, std::string topic); // TOPIC
		void	mode(char param); // MODE

		void	broadcast(const std::string &msg); // PRIVMSG / NOTICE
		void	broadcast(const std::string &msg, Client *excludeUser); // PRIVMSG / NOTICE
};
