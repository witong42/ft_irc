/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:43 by witong            #+#    #+#             */
/*   Updated: 2026/01/05 12:49:40 by witong           ###   ########.fr       */
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

void	Channel::broadcast(const std::string &msg)
{
	for (std::map<Client *, bool>::iterator it = _users.begin(); it != _users.end(); it++)
		it->first->reply(msg);
}

void	Channel::broadcast(const std::string &msg, Client *excludeUser)
{
	for (std::map<Client *, bool>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (it->first != excludeUser)
			it->first->reply(msg);
	}
}

bool Channel::handleModeI(ModeContext &ctx)
{
	setInviteOnly(ctx.adding);
	return true;
}

bool Channel::handleModeT(ModeContext &ctx)
{
	setTopicRestricted(ctx.adding);
	return true;
}

bool Channel::handleModeK(ModeContext &ctx)
{
	if (ctx.adding)
	{
		if (ctx.argIdx >= ctx.args.size())
			return false;
		std::string key = ctx.args[ctx.argIdx++];
		setKey(key);
		ctx.appliedArgs += " " + key;
	}
	else
		setKey("");
	return true;
}

bool Channel::handleModeL(ModeContext &ctx)
{
	if (ctx.adding)
	{
		if (ctx.argIdx >= ctx.args.size())
			return false;
		std::string limit = ctx.args[ctx.argIdx++];
		setLimit(std::atoi(limit.c_str()));
		ctx.appliedArgs += " " + limit;
	}
	else
		removeLimit();
	return true;
}

bool Channel::handleModeO(ModeContext &ctx)
{
	if (ctx.argIdx >= ctx.args.size())
		return false;

	std::string nick = ctx.args[ctx.argIdx++];
	Client *target = NULL;

	for (std::map<Client *, bool>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->first->getNickname() == nick)
		{
			target = it->first;
			break;
		}
	}

	if (target)
	{
		if (ctx.adding)
			addOperator(target);
		else
			removeOperator(target);
		ctx.appliedArgs += " " + nick;
		return true;
	}
	return false;
}

void Channel::processModeChar(char c, ModeContext &ctx, Client *user)
{
	if (c == '+' || c == '-')
	{
		if (c == '+')
			ctx.adding = true;
		else
			ctx.adding = false;
		return;
	}

	bool success = false;
	switch (c)
	{
		case 'i': success = handleModeI(ctx);
			break;
		case 't': success = handleModeT(ctx);
			break;
		case 'k': success = handleModeK(ctx);
			break;
		case 'l': success = handleModeL(ctx);
			break;
		case 'o': success = handleModeO(ctx);
			break;
		default:
			// RFC 2812: 472 ERR_UNKNOWNMODE
			std::string cStr(1, c);
			user->reply("472 " + user->getNickname() + " " + cStr + " :is unknown mode char to me");
			break;
	}

	if (success)
	{
		char currentSign = ctx.adding ? '+' : '-';
		if (ctx.lastSign != currentSign)
		{
			ctx.appliedModes += currentSign;
			ctx.lastSign = currentSign;
		}
		ctx.appliedModes += c;
	}
}

void Channel::sendChannelModes(Client *user)
{
	std::string modeStr = "+";
	std::string argsStr = "";

	if (isInviteOnly())
		modeStr += "i";
	if (isTopicRestricted())
		modeStr += "t";
	if (hasKey())
		modeStr += "k";
	if (hasLimit()) {
		modeStr += "l";
		std::stringstream ss;
		ss << getLimit();
		argsStr += " " + ss.str();
	}

	// 324 RPL_CHANNELMODEIS
	user->reply("324 " + user->getNickname() + " " + _name + " " + modeStr + argsStr);
}

bool Channel::checkOperatorPrivileges(Client *user)
{
	if (!isOperator(user))
	{
		// 482 ERR_CHANOPRIVSNEEDED
		user->reply("482 " + user->getNickname() + " " + _name + " :You're not channel operator");
		return false;
	}
	return true;
}

void	Channel::mode(Client *user, const std::string &modes, const std::vector<std::string> &args)
{
	// RFC 2812: If no modes given, return current modes (RPL_CHANNELMODEIS)
	if (modes.empty())
	{
		sendChannelModes(user);
		return;
	}

	// RFC 2812: Check for Operator Privileges
	if (!checkOperatorPrivileges(user))
		return;

	ModeContext ctx(args);

	for (size_t i = 0; i < modes.length(); i++)
	{
		processModeChar(modes[i], ctx, user);
	}

	if (!ctx.appliedModes.empty())
		broadcast(":" + user->getNickname() + " MODE " + _name + " " + ctx.appliedModes + ctx.appliedArgs);
}
