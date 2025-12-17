/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 11:53:18 by witong            #+#    #+#             */
/*   Updated: 2025/12/17 12:30:13 by witong           ###   ########.fr       */
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

		bool	_isOperator;
		bool	_isRegisted;

		// authenticate
		// set nickname
		// set username
		// join a channel
		// send private messages
		// receive private messages
		// operators / regular
	public:
		Client();
		Client(int fd, std::string ip);
		Client(const Client &src);
		Client &operator=(const Client &src);
		~Client();

		void	setNickname(const std::string &nickname);
		void	setUsername(const std::string &username);
		const std::string &getNickname(void) const;
		const std::string &getUsername(void) const;
};
