/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:42 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/08 20:20:55 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "../header/String.hpp"
#include "../header/Server.hpp"
#include "../header/Channel.hpp"
#include <string>

class Irc
{
private:
    std::map<String, Channel *> _channels;
	Channel* findChannel(String channel);
public:
    static bool CmdNick(std::vector<String> vector_buffer, Server server);
    static bool CmdUser(std::vector<String> vector_buffer, Server server);
    static bool CmdJoin(std::vector<String> vector_buffer, Server server);
    static bool CmdPart(std::vector<String> vector_buffer, Server server);
    static bool CmdPrivmsg(std::vector<String> vector_buffer,  Server server);
    static bool CmdPassw(std::vector<String> vector_buffer,  Server server);
};