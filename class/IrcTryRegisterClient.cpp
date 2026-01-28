/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcTryRegisterClient.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 11:03:04 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/28 11:07:10 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Irc.hpp"
#include "../header/Server.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"
#include "../header/String.hpp"
#include <iostream>
#include <vector>

bool Irc::TryRegisterClient(Client &client)
{
	if (client.isRegistered())
		return true;

	if (client.getNickname().empty() || client.getUsername().empty())
		return false;

	client.reply(RPL_WELCOME(client.getNickname(), client.getUsername(), client.getHostname()) + "\r\n");
	client.setRegistered(true);
	return true;
}