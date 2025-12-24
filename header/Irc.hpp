/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 11:33:42 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/23 19:28:18 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "String.hpp"
#include "Server.hpp"


class Irc
{
public:
    static bool CmdNick(std::vector<String> vector_buffer, Server server);
    static bool CmdUser(std::vector<String> vector_buffer, Server server);
    static bool CmdJoin(std::vector<String> vector_buffer, Server server);
    static bool CmdPart(std::vector<String> vector_buffer, Server server);
    static bool CmdPrivmsg(std::vector<String> vector_buffer,  Server server);
    static bool CmdPassw(std::vector<String> vector_buffer,  Server server);
};