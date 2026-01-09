/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:14 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/18 13:02:02 by jegirard         ###   ########.fr       */
/*                           >                                                 */
/* ************************************************************************** */

#pragma once
#include <algorithm>
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
#include <string>
#include <map>
#include <vector>
#include "../header/String.hpp"

#include "../header/Client.hpp"
#define MAX_EVENTS 10
#define BUFFER_SIZE 512

class Server
{
private:
	int _fd; // fichier de descripteur du socket
	// int _fd_client; // fichier de descripteur du socket client
	int _fd_epoll; // descripteur epoll
	int _fd_client;
	//	Client _client;
	String _password; // mot de passe du serveur
	String _SERVER_NAME;

	struct epoll_event _ev, events[MAX_EVENTS];
	struct sockaddr_in _address;
	// std::vector<Client>*	_invited;
	std::map<int, Client *> _invited;

	bool check_port(const char *port);									  // vérifie la validité du port
	bool createSocket();												  // crée le socket
	bool socketUnblock();												  // met le socket en non-bloquant
	bool IPv4bind();													  // lie le socket à une adresse IPv4
	bool listening();													  // met le socket en écoute
	bool AddSockette();													  // ajoute le socket au epoll
	bool createPoll();													  // crée le descripteur epoll
	bool wait();														  // boucle principale du serveur
	bool CleanUp();														  // nettoie les ressources utilisées
	bool parseCommand(std::string buffer, int fd);						  // parse les commandes reçues
	bool parseSwitchCommand(std::string cmd, std::string buffer, int fd); // switch pour les commandes
	bool AddClient(int fd, std::string ip);								  // ajoute un client à la liste des connectés

	Client *findInvitedByfd(int idRecherche);

public:
	Server(const char *port, String password);
	~Server();
	class InvalidPortException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Invalid port number";
		}
	};
	bool SendClientMessage(int fd_client, std::string *codes);

	Server(int port, String password);
	int &getfd();
	int &getClientFd();
	bool CheckPassword(String password, int fd);
	void Run();

	void Start();
};