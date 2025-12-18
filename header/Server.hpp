/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:14 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/18 11:33:43 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstring>
#include <cstdlib>
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>



class Server
{
private:
	int _port; //port utilisé par le serveur
	int _fd; // fichier de descripteur du socket
	//int _fd_client; // fichier de descripteur du socket client
	int _fd_epoll; // descripteur epoll
	const char *_password; // mot de passe du serveur
	int _domaine ; // domaine IPv4
	int _type ; // type de socket TCP
	struct epoll_event _ev, events[MAX_EVENTS];
	
	bool check_port(const char *port); // vérifie la validité du port
	bool createSocket();	// crée le socket
	bool socketUnblock(); // met le socket en non-bloquant
	bool IPv4bind(); // lie le socket à une adresse IPv4
	bool AddSockette(); // ajoute le socket au epoll
	bool Lisen(); // boucle principale du serveur
	bool CleanUp(); // nettoie les ressources utilisées

public:
	Server(const char *port, const char *password);
	~Server();
	class InvalidPortException : public std::exception
	{
		virtual const char* what() const throw()
		{
			return "Invalid port number";
		}
	};
	Server(int port, const char *password);
	void run();

	void start();
};