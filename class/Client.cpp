/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:38 by witong            #+#    #+#             */
/*   Updated: 2026/02/02 11:55:47 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <cerrno>
#include "../header/Client.hpp"
#include "../header/Logger.hpp"

Client::Client() : _fd(-1), _ip(""), _nickname(""), _username(""), _readBuffer(""), _sendBuffer(""), _isOperator(false), _isRegistered(false), _isPwdOk(false), _shouldDisconnect(false)
{
}

Client::Client(int fd, std::string ip) : _fd(fd), _ip(ip), _nickname(""), _username(""), _readBuffer(""), _sendBuffer(""), _isOperator(false), _isRegistered(false), _isPwdOk(false), _shouldDisconnect(false)
{
}

Client::Client(const Client &other)
{
	*this = other;
}

Client &Client::operator=(const Client &rhs)
{
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_ip = rhs._ip;
		this->_nickname = rhs._nickname;
		this->_username = rhs._username;
		this->_readBuffer = rhs._readBuffer;
		this->_sendBuffer = rhs._sendBuffer;
		this->_isOperator = rhs._isOperator;
		this->_isRegistered = rhs._isRegistered;
		this->_isPwdOk = rhs._isPwdOk;
		this->_shouldDisconnect = rhs._shouldDisconnect;
	}
	return (*this);
}

Client::~Client()
{
	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}
}

void Client::setNickname(const std::string &nickname)
{
	this->_nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
	this->_username = username;
}

void Client::setRegistered(bool status)
{
	this->_isRegistered = status;
}

void Client::setOperator(bool status)
{
	this->_isOperator = status;
}

void Client::setShouldDisconnect(bool status)
{
	this->_shouldDisconnect = status;
}

// Getters

int Client::getFd() const
{
	return (this->_fd);
}

const std::string &Client::getIp() const
{
	return this->_ip;
}

const std::string &Client::getNickname(void) const
{
	return this->_nickname;
}

const std::string &Client::getUsername(void) const
{
	return this->_username;
}

std::string &Client::getReadBuffer()
{
	return this->_readBuffer;
}

bool Client::isRegistered() const
{
	return this->_isRegistered;
}

bool Client::isOperator() const
{
	return (this->_isOperator);
}

bool Client::getShouldDisconnect() const
{
	return (this->_shouldDisconnect);
}

const std::string &Client::getHostname() const
{
	return (this->_hostname);
}

void Client::appendReadBuffer(const std::string &chunk)
{
	this->_readBuffer += chunk;
}

void Client::clearReadBuffer()
{
	this->_readBuffer.clear();
}

void Client::reply(const std::string &msg)
{
	if (this->_sendBuffer.length() + msg.length() > 100000)
	{
		Logger::error("Buffer Limit for client " + this->_nickname);
		return;
	}
	Logger::debug("Sending to " + _nickname + ": " + msg);
	this->_sendBuffer += msg + "\r\n";
}

void Client::flush()
{
	if (this->_sendBuffer.empty())
		return;
	while (!this->_sendBuffer.empty())
	{
		ssize_t bytesSent = send(this->_fd, this->_sendBuffer.c_str(), this->_sendBuffer.length(), 0);
		if (bytesSent == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				Logger::error("Failed to send message to client " + this->_nickname);
			}
			break;
		}
		else
		{
			if (static_cast<size_t>(bytesSent) >= this->_sendBuffer.length())
				this->_sendBuffer.clear();
			else
				this->_sendBuffer = this->_sendBuffer.substr(bytesSent);
		}
	}
}

bool Client::hasPendingWrites() const
{
	return !this->_sendBuffer.empty();
}

bool Client::isPwdOk() const
{
	return this->_isPwdOk;
}

void Client::setPwdOk(bool status)
{
	this->_isPwdOk = status;
}
