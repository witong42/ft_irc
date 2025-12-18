/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:18 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/18 19:01:20 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>

#include <iostream>
#include "Server.hpp"
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
	
#include <arpa/inet.h>
#include <errno.h>



Server::Server(int port, const char *password)
{
	if (port < 1 || port > 65535)
	{
		throw std::invalid_argument("Invalid port number");
	}
	
	_password = password;
	
	// Initialisation de l'adresse
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_port = htons(port);
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	
	// Constructor implementation
}

void Server::run()
{
	if (!createSocket())
	{
		throw std::runtime_error("Socket creation failed");
	}
	if (!socketUnblock())
	{
		throw std::runtime_error("Setting socket to non-blocking failed");
	}
	if (!IPv4bind())
	{
		throw std::runtime_error("Binding socket failed");
	}
	// Listening
	if (!listening())
	{
		throw std::runtime_error("Listening on socket failed");
	}
	if (!createPoll())
	{
		throw std::runtime_error("Creating epoll instance failed");
	}
	if (!AddSockette())
	{
		throw std::runtime_error("Adding socket to epoll failed");
	}
	if (!wait())
	{
		throw std::runtime_error("Server wait loop failed");
	}
	CleanUp();
}

bool Server::createSocket()
{
	// Crée la socket
	_fd = socket(_address.sin_family, SOCK_STREAM, 0);
	if (_fd < 0){

		std::cerr << "Erreur création socket\n";
		return false;
	}
	
	// Configurer SO_REUSEADDR
	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Erreur setsockopt\n";
		close(_fd);
		return false;
	}
	return true;
}

bool Server::socketUnblock()
{
	// Set socket to non-blocking implementation
	int flags = fcntl(_fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return false;
	return true;
}

bool Server::IPv4bind()
{
	// IPv4 bind implementation
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		std::cerr << "Erreur bind\n";
		close(_fd);
		return false;
	}
	return true;
}
bool Server::listening(){
	// Écoute des connexions entrantes
	if (listen(_fd, 10) < 0)
	{
		std::cerr << "Erreur listen\n";
		close(_fd);
		return false;
	}
	std::cout << "Serveur en écoute sur le port " << ntohs(_address.sin_port) << "...\n";
	return true;
}


bool Server::createPoll(){	
	// IPv4 bind implementation
	_fd_epoll = epoll_create1(0);
	if (_fd_epoll == -1)
	{
		std::cerr << "Erreur epoll_create1\n";
		close(_fd);
		return false;
	}
	return true;
}

bool Server::AddSockette(){
	
	// 5. Ajouter le socket serveur à epoll
	_ev.events = EPOLLIN; // Surveiller les événements de lecture
	_ev.data.fd = _fd;
	// Ajouter le socket serveur à epoll
	if (epoll_ctl(_fd_epoll, EPOLL_CTL_ADD, _fd, &_ev) < -1)
	{
		std::cerr << "Erreur epoll_ctl\n";
		close(_fd);
		close(_fd_epoll);
		return false;
	}
	return true;
}

bool Server::wait(){
	

	// 6. Boucle principale
	while (true)
	{
		int nfds = epoll_wait(_fd_epoll, events, MAX_EVENTS, -1);
		if (nfds < 0)
		{
			std::cerr << "Erreur epoll_wait\n";
			break;
		}

		// Traiter tous les événements
		for (int i = 0; i < nfds; i++)
		{
			// Nouvelle connexion sur le socket serveur
			if (events[i].data.fd == _fd)
			{
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);

				int _fd_client = accept(_fd, (struct sockaddr *)&client_addr, &client_len);
				if (_fd_client == -1)
				{
					if (errno != EAGAIN && errno != EWOULDBLOCK)
					{
						std::cerr << "Erreur accept\n";
					}
					continue;
				}

				// Afficher info client
				char client_ip[INET_ADDRSTRLEN];
				inet_ntop(_address.sin_family, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
				std::cout << "Nouvelle connexion de " << client_ip
						  << ":" << ntohs(client_addr.sin_port) << "\n";
				std::cout << "Client fd: " << _fd_client << "\n";

				// Rendre le socket client non-bloquant
				socketUnblock();

				// Ajouter le client à epoll
				_ev.events = EPOLLIN | EPOLLET; // Edge-triggered
				_ev.data.fd = _fd_client;
				if (epoll_ctl(_fd_epoll, EPOLL_CTL_ADD, _fd_client, &_ev) == -1)
				{
					std::cerr << "Erreur ajout client à epoll\n";
					close(_fd_client);
				}
			}
			// Données disponibles sur un socket client
			else
			{
				int _fd_client = events[i].data.fd;
				char buffer[BUFFER_SIZE];

				ssize_t count = recv(_fd_client, buffer, sizeof(buffer) - 1, 0);

				if (count == -1)
				{
					if (errno != EAGAIN)
					{
						std::cerr << "Erreur recv\n";
						epoll_ctl(_fd_epoll, EPOLL_CTL_DEL, _fd_client, NULL);
						close(_fd_client);
					}
				}
				else if (count == 0)
				{
					// Client a fermé la connexion
					std::cout << "Client déconnecté (fd: " << _fd_client << ")\n";
					epoll_ctl(_fd_epoll, EPOLL_CTL_DEL, _fd_client, NULL);
					close(_fd_client);
				}
				else
				{
					// Afficher les données reçues
					buffer[count] = '\0';
					std::cout << "Reçu (" << count << " octets): " << buffer;

					// Echo - renvoyer les données au client
					//send(_fd_client, buffer, count, 0);

					    std::string reply = ":localhost 001 jegirard : Welcome to the ft_irc server!\r\n";

    // On envoie la réponse au client
    if (send(_fd_client, reply.c_str(), reply.length(), 0) < 0) {
        std::cerr << "Erreur send()" << std::endl;
    }
				

					//close(_fd_client);


					
				}
			}
		}
	}
 return true;
}

bool Server::CleanUp(){
	// Nettoyage
	close(_fd);
	close(_fd_epoll);
	return 0;
}

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



Server::Server(const char *port, const char *password)
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