/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:38 by witong            #+#    #+#             */
/*   Updated: 2026/01/12 13:41:19 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Client.hpp"

Client::Client() : _fd(-1), _ip(""), _nickname(""), _username(""), _buffer(""), _isOperator(false), _isRegistered(false)
{
}

Client::Client(int fd, std::string ip) : _fd(fd), _ip(ip), _nickname(""), _username(""), _buffer(""), _isOperator(false), _isRegistered(false)
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
		_fd = rhs._fd;
		_ip = rhs._ip;
		_nickname = rhs._nickname;
		_username = rhs._username;
		_buffer = rhs._buffer;
		_isOperator = rhs._isOperator;
		_isRegistered = rhs._isRegistered;
	}
	return *this;
}

Client::~Client()
{
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

int Client::getFd() const
{
	return this->_fd;
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

std::string Client::getBuffer() const
{
	return this->_buffer;
}

bool Client::isRegistered() const
{
	return this->_isRegistered;
}

bool Client::isOperator() const
{
	return this->_isOperator;
}

void Client::appendBuffer(const std::string &chunk)
{
	this->_buffer += chunk;
}

void Client::clearBuffer()
{
	this->_buffer.clear();
}

void Client::reply(const std::string &msg)
{
	std::string finalString = msg + "\r\n";
	std::cout << "Sending to client " << this->_fd << ": " << finalString;	
	if (send(this->_fd, finalString.c_str(), finalString.length(), 0) == -1)
		std::cerr << "Error: Failed to send message to client " << this->_fd << std::endl;
}
bool Client::message(std::string *codes)
{
	std::string message = ":localhost " + codes[0] + " jegirard : Welcome to the ft_irc server!\r\n";
	for (size_t i = 1; i < codes->size(); ++i)
	{
		message += ":localhost " + codes[i] + " jegirard : This is a sample message for code " + codes[i] + "\r\n";
	}
	if (send(_fd, message.c_str(), message.length(), 0) < 0)
	{
		std::cerr << "Erreur send()" << std::endl;
		return false;
	}
	return true;
}