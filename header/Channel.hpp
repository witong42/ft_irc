/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:52:56 by witong            #+#    #+#             */
/*   Updated: 2025/12/23 11:38:45 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include "Client.hpp"

class Client;

class Channel
{
	private:
		std::string		_name;
		std::string				_topic;

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
		void	setTopic(const std::string &topic);
		void	setInviteOnly(bool status);
		void	setTopicRestricted(bool status);
		void	setKey(const std::string &key);
		void	setLimit(size_t limit);
		void	removeLimit();

		// Getters
		const std::string	&getName() const;
		const std::string	&getTopic() const;
		const std::string	&getKey() const;
		size_t				getLimit() const;
		size_t				getUserCount() const;
		std::string			getUserList() const; // needed for join

		// Mode status
		bool	isOperator(Client *user) const;
		bool	isInviteOnly() const;
		bool	isTopicRestricted() const;
		bool	hasLimit() const;
		bool	hasKey() const;
		bool	hasUser(Client *user) const;

		// Mode operations
		void	addOperator(Client *user);
		void	removeOperator(Client *user);

		// Actions
		void	join(Client *user);
		void	removeUser(Client *user);
		void	kick(Client* kicker, Client *user);
		void	invite(Client *user);
		void	changeTopic(Client *user, std::string topic);
		void	mode(char param);

		void	broadcast(const std::string &msg);
		void	broadcast(const std::string &msg, Client *excludeUser);
};
