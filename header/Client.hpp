/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:18 by witong            #+#    #+#             */
/*   Updated: 2025/12/19 09:52:03 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Client
{
	private:
		int _fd;
		std::string _ip;

		std::string _nickname;
		std::string _username;

		std::string _buffer;
		bool	_isOperator;
		bool	_isRegisted;

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
		void				setRegisted(bool status);
		void				setOperator(bool status);

		// Getters
		const std::string	&getNickname(void) const;
		const std::string	&getUsername(void) const;
		std::string			getBuffer() const;
		bool				isRegistered() const;
		bool				isOperator() const;

		// Methods
		void				appendBuffer(const std::string &chunk);	// To store incoming data
		void				clearBuffer();							// To wipe buffer after processing
		void				reply(const std::string &msg);			// To write to the socket (_fd)
};
