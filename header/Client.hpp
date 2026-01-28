/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:18 by witong            #+#    #+#             */
/*   Updated: 2026/01/28 09:43:40 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <cerrno>
#include <unistd.h>

class Client
{
private:
	int _fd;
	std::string _ip;

	std::string _nickname;
	std::string _username;

	std::string _readBuffer;
	std::string _sendBuffer;
	std::string _hostname;
	bool _isOperator;
	bool _isRegistered;

public:
	// Canonical Form
	Client();
	Client(int fd, std::string ip);
	Client(const Client &other);
	Client &operator=(const Client &rhs);
	~Client();

	// Setters
	void setNickname(const std::string &nickname);
	void setUsername(const std::string &username);
	void setRegistered(bool status);
	void setOperator(bool status);
	void setHostname(const std::string &hostname);

	bool hasPendingWrites() const;

	// Getters
	int getFd() const;
	const std::string &getIp() const;
	const std::string &getHostname() const;
	const std::string &getNickname() const;
	const std::string &getUsername() const;

	std::string &getReadBuffer();
	bool isRegistered() const;
	bool isOperator() const;

	// Methods
	void appendReadBuffer(const std::string &chunk); // To store incoming data
	void clearReadBuffer();							 // To wipe buffer after processing
	void reply(const std::string &msg);			 // To queue message to send buffer
	void flush();								 // To write queued messages to socket
};

#endif
