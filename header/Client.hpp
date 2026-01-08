/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:18 by witong            #+#    #+#             */
/*   Updated: 2026/01/08 15:58:34 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <sys/socket.h>

class Client
{
	private:
		int _fd;
		std::string _ip;

		std::string _nickname;
		std::string _username;

		std::string _buffer;
		bool	_isOperator;
		bool	_isRegistered;

	public:
		// Canonical Form
		Client();
		Client(int fd, std::string ip);
		Client(const Client &other);
		Client &operator=(const Client &rhs);
		~Client();

		// Setters
		void				setNickname(const std::string &nickname);
		void				setUsername(const std::string &username);
		void				setRegistered(bool status);
		void				setOperator(bool status);

		// Getters
		int					getFd() const;
		const std::string	&getIp() const;
		const std::string	&getNickname() const;
		const std::string	&getUsername() const;
		std::string			getBuffer() const;
		bool				isRegistered() const;
		bool				isOperator() const;

		// Methods
		void				appendBuffer(const std::string &chunk);	// To store incoming data
		void				clearBuffer();							// To wipe buffer after processing
		void				reply(const std::string &msg);			// To write to the socket (_fd)
		bool 				SendClientMessage(std::string* codes);
};
