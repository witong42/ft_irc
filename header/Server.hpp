/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:14 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/23 10:40:00 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <queue>
#include "../header/String.hpp"
#include "../header/Client.hpp"

#define MAX_EVENTS 10
#define BUFFER_SIZE 512

class Server
{
private:
	int _fd_server; // fichier de descripteur du socket
	// int _fd_client; // fichier de descripteur du socket client
	int _fd_epoll; // descripteur epoll
	int _fd_client;
	//	Client _client;
	String _password; // mot de passe du serveur
	String _SERVER_NAME;

	struct epoll_event _ev, events[MAX_EVENTS];
	struct sockaddr_in _address;
	std::map<int, Client *> _connected_clients; // liste des clients connectés
	std::vector<std::string> _messages_queue;	// file d'attente des messages à envoyer aux clients

	std::map<int, std::queue<std::string> > _out_queues; // file d'attente des messages par client

	int check_port(const char *port);		// vérifie la validité du port
	bool createSocket();					// crée le socket
	bool socketUnblock(int fd);				// met le socket en non-bloquant
	bool IPv4bind();						// lie le socket à une adresse IPv4
	bool listening();						// met le socket en écoute
	bool AddSocket();						// ajoute le socket au epoll
	bool createPoll();						// crée le descripteur epoll
	bool wait();							// boucle principale du serveur
	bool CleanUp();							// nettoie les ressources utilisées
	bool AddClient(int fd, std::string ip); // ajoute un client à la liste des connectés
	void sendPendingMessages(int fd);		// envoie les messages en attente à un client

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
	Client *findConnectedByfd(int idRecherche);
	Client *findConnectedByNickname(String Nickname);
	Client *findConnectedByUsername(String Username);
	String getServerName();
	int &getServerFd();
	int &getClientFd();
	bool CheckPassword(String password, int fd);
	void Run();
	int getQueuesSize();

	void Start();
};

#endif
