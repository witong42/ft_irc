/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:18 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/15 15:22:34 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>

#include <iostream>
#include "server.hpp"
#include <cctype>
#include <cstring>
#include <stdexcept>

bool Server::check_port(const char *port)
{
	int len = std::strlen(port);
	for (int i = 0; i < len; i++)
	{
		if (!isdigit(port[i]))
			return false;
	}
	int port_num = std::atoi(port);
	if (port_num < 1 || port_num > 65535)
		return false;
	return true;
}

Server::Server(int port, const char* password)
{
    if (port < 1 || port > 65535)
    {
        throw std::invalid_argument("Invalid port number");
    }
    this->_port = port;
    this->_password = password;
    // Constructor implementation
    
}

Server::Server(const char* port, const char* password)
{
    if (!check_port(port))
    {
        throw std::invalid_argument("Invalid port number");
    }
    Server(std::atoi(port), password);
    // Constructor implementation
    
}
Server::~Server()
{
    // Destructor implementation
}
void Server::start()
{
    // Start server implementation
}