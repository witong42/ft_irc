/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:43 by witong            #+#    #+#             */
/*   Updated: 2026/01/08 14:02:34 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Channel.hpp"


Channel::Channel(const std::string &name, Client *creator) : _name(name)
{
	_users[creator] = true; // Creator is an operator
	_topic = "No topic set";
	_isInviteOnly = false;
	_isTopicRestricted = false;
	_key = "";
	_limit = 0;
	_hasLimit = false;
}

Channel::~Channel()
{
}
Channel::Channel(const Channel &other) : _name(other._name), _topic(other._topic), _users(other._users), _invited(other._invited),
	_isInviteOnly(other._isInviteOnly), _isTopicRestricted(other._isTopicRestricted), _key(other._key), _limit(other._limit), _hasLimit(other._hasLimit)
{
}

Channel &Channel::operator=(const Channel &rhs)
{
	if (this != &rhs)
	{
		_name = rhs._name;
		_topic = rhs._topic;
		_users = rhs._users;
		_invited = rhs._invited;
		_isInviteOnly = rhs._isInviteOnly;
		_isTopicRestricted = rhs._isTopicRestricted;
		_key = rhs._key;
		_limit = rhs._limit;
		_hasLimit = rhs._hasLimit;
	}
	return *this;
}

void Channel::setTopic(const std::string &topic)
{
	_topic = topic;
}
void	Channel::setTopicRestricted(bool status)
{
	_isTopicRestricted = status;
}

void	Channel::setKey(const std::string &key)
{
_key = key;
	
}
void	Channel::setLimit(size_t limit)	
{
	_limit = limit;
	_hasLimit = true;
}

void	Channel::removeLimit()
{
	_hasLimit = false;
}

const std::string &Channel::getName() const
{
	return _name;
}


const std::string &Channel::getTopic() const
{
	return _topic;
}

const std::string &Channel::getKey() const
{
	return _key;
}


size_t	Channel::getLimit() const
{

	return _limit;
}

size_t Channel::getUserCount() const
{
	return _users.size();
}

std::string Channel::getUserList() const
{
	std::string userList;
	for (std::map<Client *, bool>::const_iterator it = _users.begin(); it != _users.end(); ++it)
	{
		Client *user = it->first;
		if (it != _users.begin())
			userList += " ";
		userList += user->getNickname();
	}
	return userList;
}
bool Channel::isOperator(Client *user) const
{
	std::map<Client *, bool>::const_iterator it = _users.find(user);
	if (it != _users.end())
		return it->second;
	return false;
}
void Channel::addOperator(Client *user)
{
	std::map<Client *, bool>::iterator it = _users.find(user);
	if (it != _users.end())
		it->second = true;
}	
void Channel::removeOperator(Client *user)
{
	std::map<Client *, bool>::iterator it = _users.find(user);
	if (it != _users.end())
		it->second = false;
}



void Channel::join(Client *user)
{
	_users[user] = false; // By default, user is not an operator
}
void Channel::removeUser(Client *user)
{
	_users.erase(user);
}

void Channel::kick(Client* kicker, Client *user)
{
	if (hasUser(user))
	{
		std::string kickMessage = ":" + kicker->getNickname() + " KICK " + _name + " " + user->getNickname() + " :You have been kicked from the channel.";
		broadcast(kickMessage);
		removeUser(user);
	}
}


bool Channel::hasUser(Client *user) const
{
	return _users.find(user) != _users.end();
}



void Channel::invite(Client *user)
{
	_invited.push_back(user);
}
void Channel::changeTopic(Client *user, std::string topic)
{
	_topic = topic;
	std::string topicMessage = ":" + user->getNickname() + " TOPIC " + _name + " :" + topic;
	broadcast(topicMessage);
}




void Channel::broadcast(const std::string &msg)
{
	for (std::map<Client *, bool>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		Client *user = it->first;
		user->reply(msg);
	}
}
void Channel::broadcast(const std::string &msg, Client *excludeUser)
{
	for (std::map<Client *, bool>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		Client *user = it->first;
		if (user != excludeUser)
			user->reply(msg);
	}
}



void	Channel::setInviteOnly(bool status)
{
	_isInviteOnly = status;
}
bool	Channel::isInviteOnly() const
{
	return _isInviteOnly;
}

bool	Channel::isTopicRestricted() const
{
	return _isTopicRestricted;
}


bool	Channel::hasKey() const
{
	return !_key.empty();
}

bool	Channel::hasLimit() const
{
	return _hasLimit;
}



	
