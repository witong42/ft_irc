/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:52:56 by witong            #+#    #+#             */
/*   Updated: 2025/12/19 12:34:09 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include "Client.hpp"

class Client;

class Channel
{
	private:
		std::string		_name;
		std::string				_topic;

		// first bool is operator, second bool is invite
		std::map<Client *, std::pair<bool, bool> >	_users;

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

		// Getters
		const std::string	&getName() const;
		const std::string	&getTopic() const;
		const std::string	&getKey() const;
		size_t				getLimit() const;

		// Mode status
		bool				isInviteOnly() const;
		bool				isTopicRestricted() const;
		bool				hasLimit() const;
		bool				hasKey() const;

		// Methods
		void	kick(Client* kicker, Client *user);
		void	invite(Client *user);
		void	changeTopic(Client *user, std::string topic);
		void	mode(char param);
		void	join(Client *user);
		void	sendPriv(Client *user, std::string msg);
		void	receivePrive(Client *user, std::string mag);
};
