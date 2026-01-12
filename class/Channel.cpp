/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:43 by witong            #+#    #+#             */
/*   Updated: 2026/01/12 15:46:49 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Channel.hpp"
#include "../header/Client.hpp"

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

void Channel::setTopic(const std::string &topic)
{
	this->_topic = topic;
}

void Channel::setInviteOnly(bool status)
{
	this->_isInviteOnly = status;
}

void Channel::setTopicRestricted(bool status)
{
	this->_isTopicRestricted = status;
}

void Channel::setKey(const std::string &key)
{
	this->_key = key;
}

void Channel::setLimit(size_t limit)
{
	this->_limit = limit;
	this->_hasLimit = true;
}

void Channel::removeLimit()
{
	this->_limit = 0;
	this->_hasLimit = false;
}

const std::string &Channel::getName() const
{
	return this->_name;
}

const std::string &Channel::getTopic() const
{
	return this->_topic;
}

const std::string &Channel::getKey() const
{
	return this->_key;
}

size_t Channel::getLimit() const
{
	return this->_limit;
}

size_t Channel::getUserCount() const
{
	return this->_users.size();
}

std::string Channel::getUserList() const
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

bool Channel::isOperator(Client *user) const
{
	std::map<Client *, bool>::const_iterator it = this->_users.find(user);
	if (it != this->_users.end())
		return it->second;
	return false;
}

bool Channel::isInviteOnly() const
{
	return this->_isInviteOnly;
}

bool Channel::isTopicRestricted() const
{
	return this->_isTopicRestricted;
}

bool Channel::hasLimit() const
{
	return this->_hasLimit;
}

bool Channel::hasKey() const
{
	return !this->_key.empty();
}

bool Channel::hasUser(Client *user) const
{
	std::map<Client *, bool>::const_iterator it = this->_users.find(user);
	if (it != this->_users.end())
		return true;
	return false;
}

void Channel::addOperator(Client *user)
{
	this->_users[user] = true;
}

void Channel::removeOperator(Client *user)
{
	if (this->_users.find(user) != this->_users.end())
		this->_users[user] = false;
}

void Channel::addUser(Client *user)
{
	this->_users[user] = false;
}

void Channel::removeUser(Client *user)
{
	if (this->_users.find(user) != this->_users.end())
		this->_users.erase(user);
}

void Channel::invite(Client *user)
{
	if (std::find(this->_invited.begin(), this->_invited.end(), user) == this->_invited.end())
		this->_invited.push_back(user);
}

void Channel::kick(Client *kicker, Client *user)
{
	if (this->isOperator(kicker))
		this->removeUser(user);
}

void Channel::changeTopic(Client *user, std::string topic)
{
	if (!this->isTopicRestricted() || this->isOperator(user))
		this->_topic = topic;
}

void Channel::broadcast(const std::string &msg)
{
	std::cout << "Broadcasting message to channel " << this->_name << ": " << msg << std::endl;
	std::cout << "Number of users in channel: " << this->_users.size() << std::endl;
	for (std::map<Client *, bool>::iterator it = _users.begin(); it != _users.end(); it++)
		it->first->reply(msg);
}

void Channel::broadcast(const std::string &msg, Client *excludeUser)
{
	for (std::map<Client *, bool>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (it->first != excludeUser)
			it->first->reply(msg);
	}
}

// STOPPED HERE

// void	Channel::mode(char param)
// {
// 	(void)param;
// }
