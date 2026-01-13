/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:18 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/13 10:44:56 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <iostream>
#include <string>
#include <iostream>

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
#include <sstream>
#include <vector>
#include <map>
#include "String.hpp"
#include "../header/Server.hpp"
#include "Irc.hpp"
#include "Client.hpp"

// Example command to test: irssi
// sev IRC
// /connect localhost 6667 pwd123

Server::Server(int port, String password)
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
Server::Server(const char *port, String password)
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
void Server::Start()
{
	// Start server implementation
}

int &Server::getfd()
{
	return _fd;
}
int &Server::getClientFd()
{
	return _fd_client;
}
void Server::Run()
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
	if (!AddSocket())
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
	if (_fd < 0)
	{

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
bool Server::listening()
{
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

bool Server::createPoll()
{
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

bool Server::AddSocket()
{
	// Ajouter le socket serveur à epoll
	_ev.events = EPOLLIN; // Surveiller les événements de lecture
	_ev.data.fd = _fd;	  // Ajouter le socket serveur à epoll
	if (epoll_ctl(_fd_epoll, EPOLL_CTL_ADD, _fd, &_ev) < -1)
	{
		std::cerr << "Erreur epoll_ctl\n";
		close(_fd);
		close(_fd_epoll);
		return false;
	}
	return true;
}

bool Server::CheckPassword(String password, int fd)
{

	if (password == _password)
	{
		// Here you would typically check the password against the server's password
		std::cout << "Password accepted for fd: " << password;
		std::cout << "Received PASS command with password: " << _password << " from fd: " << _fd << std::endl;
		std::string reply = ":localhost 001 jegirard : Welcome to the ft_irc server!\r\n";

		// On envoie la réponse au client

		std::string codes[4] = {"001", "002", "003", "004"};
		std::cout << "Sending welcome messages to fd: " << fd << std::endl;
		std::cout << "Codes to send: ";
		std::cout << std::endl;
		if (!(*findInvitedByfd(_fd_client)).message(codes))
		{
			std::cerr << "Erreur send()" << std::endl;
			return true;
		}

		return false;
	}
	std::cerr << "Invalid PASS command format from fd: " << _fd << std::endl;
	return true;
}

Client *Server::findInvitedByfd(int idRecherche)
{
	std::map<int, Client *>::iterator it = _connected_clients.find(idRecherche);
	std::cout << "Searching for client with fd: " << idRecherche << std::endl;
	std::cout << "Current connected clients count: " << _connected_clients.size() << std::endl;
	std::cout << "Client found: " << (it != _connected_clients.end() ? "Yes" : "No") << std::endl;
	std::cout << "Client pointer: " << (it != _connected_clients.end() ? it->second : NULL) << std::endl;
	std::cout << "Client fd: " << (it != _connected_clients.end() ? it->second->getFd() : -1) << std::endl;
	return (it != _connected_clients.end()) ? it->second : NULL;
}

bool Server::SendClientMessage(int fd_client, std::string *codes)
{
	std::string message = ":localhost  " + codes[0] + " jegirard : Welcome to the ft_irc server!\r\n";
	for (size_t i = 1; i < codes->size(); ++i)
	{
		message += ":localhost " + codes[i] + " jegirard : This is a sample message for code " + codes[i] + "\r\n";
	}
	if (send(fd_client, message.c_str(), message.length(), 0) < 0)
	{
		std::cerr << "Erreur send()" << std::endl;
		return false;
	}
	return true;
}

bool Server::CleanUp()
{
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

bool Server::wait()
{

	// 6. Boucle principale
	events->events = EPOLLIN | EPOLLET; // Edge-triggered
	Irc irc = Irc();
	while (true)
	{
		std::cout << "316 Waiting for events...\n";
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

				_fd_client = accept(_fd, (struct sockaddr *)&client_addr, &client_len);

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
				AddClient(_fd_client, client_ip);

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
				_fd_client = events[i].data.fd;
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
					// Traiter les données reçues
					buffer[count] = '\0';
					// parseCommand(std::string(buffer), _fd_client);
					irc.parseCommand(buffer, *this);

					std::cout << "392 Received from fd " << _fd_client << ": " << buffer << std::endl;
					buffer[0] = 0;
				}
			}
			if (events->events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
			{
				close(_fd_client);
			}
		}
	}
	return true;
}
bool Server::AddClient(int fd, std::string ip)
{
	Client *newClient;
	std::cout << "Adding client with fd: " << fd << " and IP: " << ip << std::endl;
	newClient = new Client(fd, ip);
	_connected_clients[fd] = newClient;
	std::cout << "Client added. Current number of invited clients: " << _connected_clients.size() << std::endl;
	std::cout << "Client fd: " << newClient->getFd() << std::endl;
	
	return newClient->isRegistered();

	// Here you would typically create a new Client object and add it to the _connected vector
}
