/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:43 by witong            #+#    #+#             */
/*   Updated: 2025/12/24 11:46:37 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Channel.hpp"

Channel::Channel() : _name(""), _topic(""), _isInviteOnly(false), _isTopicRestricted(false), _key(""), _limit(0), _hasLimit(false)
{
}

Channel::Channel(const std::string &name, Client *creator) : _name(name), _topic(""), _isInviteOnly(false), _isTopicRestricted(false), _key(""), _limit(0), _hasLimit(false)
{
	if (creator)
		addOperator(creator);
}

Channel::Channel(const Channel &other)
{
	*this = other;
}

Channel &Channel::operator=(const Channel &rhs)
{
	if (this != &rhs)
	{
		this->_name = rhs._name;
		this->_topic = rhs._topic;
		this->_users = rhs._users;
		this->_invited = rhs._invited;
		this->_isInviteOnly = rhs._isInviteOnly;
		this->_isTopicRestricted = rhs._isTopicRestricted;
		this->_key = rhs._key;
		this->_limit = rhs._limit;
		this->_hasLimit = rhs._hasLimit;
	}
	return *this;
}

Channel::~Channel()
{
}

void	Channel::setTopic(const std::string &topic)
{
	this->_topic = topic;
}

void	Channel::setInviteOnly(bool status)
{
	this->_isInviteOnly = status;
}

void	Channel::setTopicRestricted(bool status)
{
	this->_isTopicRestricted = status;
}

void	Channel::setKey(const std::string &key)
{
	this->_key = key;
}

void	Channel::setLimit(size_t limit)
{
	this->_limit = limit;
	this->_hasLimit = true;
}

void	Channel::removeLimit()
{
	this->_limit = 0;
	this->_hasLimit = false;
}

const std::string	&Channel::getName() const
{
	return this->_name;
}

const std::string	&Channel::getTopic() const
{
	return this->_topic;
}

const std::string	&Channel::getKey() const
{
	return this->_key;
}

size_t	Channel::getLimit() const
{
	return this->_limit;
}

size_t	Channel::getUserCount() const
{
	return this->_users.size();
}

std::string	Channel::getUserList() const
{
	std::string list;
	std::map<Client *, bool>::const_iterator it;
	for (it = _users.begin(); it != _users.end(); it++)
	{
		if (it != _users.begin())
			list += " ";
		if (it->second)
			list += "@";
		list += it->first->getNickname();
	}
	return list;
}

// STOPPED HERE

// bool	Channel::isOperator(Client *user) const
// {
// 	(void)user;
// 	return false;
// }

// bool	Channel::isInviteOnly() const
// {
// 	return _isInviteOnly;
// }

// bool	Channel::isTopicRestricted() const
// {
// 	return _isTopicRestricted;
// }

// bool	Channel::hasLimit() const
// {
// 	return _hasLimit;
// }

// bool	Channel::hasKey() const
// {
// 	return !_key.empty();
// }

// bool	Channel::hasUser(Client *user) const
// {
// 	(void)user;
// 	return false;
// }

// void	Channel::addOperator(Client *user)
// {
// 	(void)user;
// }

// void	Channel::removeOperator(Client *user)
// {
// 	(void)user;
// }

// void	Channel::join(Client *user)
// {
// 	(void)user;
// }

// void	Channel::removeUser(Client *user)
// {
// 	(void)user;
// }

// void	Channel::kick(Client* kicker, Client *user)
// {
// 	(void)kicker;
// 	(void)user;
// }

// void	Channel::invite(Client *user)
// {
// 	(void)user;
// }

// void	Channel::changeTopic(Client *user, std::string topic)
// {
// 	(void)user;
// 	(void)topic;
// }

// void	Channel::mode(char param)
// {
// 	(void)param;
// }

// void	Channel::broadcast(const std::string &msg)
// {
// 	(void)msg;
// }

// void	Channel::broadcast(const std::string &msg, Client *excludeUser)
// {
// 	(void)msg;
// 	(void)excludeUser;
// }
