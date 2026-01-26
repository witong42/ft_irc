/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:18 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/26 14:10:37 by jegirard         ###   ########.fr       */
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
#include "../header/String.hpp"
#include "../header/Server.hpp"
#include "../header/Irc.hpp"
#include "../header/Client.hpp"
#include "../header/Replies.hpp"

// Example command to test: irssi
// sev IRC
// /connect localhost 6667 pwd123

bool Server::_running = true;

Server::Server(const char *port_char, String password) : _password(password)
{
	int port = check_port(port_char);
	// Initialisation de l'adresse
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_port = htons(port);
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	char hostname[256];
	gethostname(hostname, sizeof(hostname));
	_server_name = String(hostname);
	std::cout << "Server name: " << _server_name << std::endl;
	// Constructor implementation
}

Server::~Server()
{
	// Destructor implementation
	std::cout << "Cleaning up server resources..." << std::endl;

	for (std::map<int, Client *>::iterator it = _connected_clients.begin(); it != _connected_clients.end(); ++it)
	{
		if (it->second)
			delete it->second;
		if (it->first)
			close(it->first);
	}
	_connected_clients.clear();
	for (_ev.data.fd = 0; _ev.data.fd < MAX_EVENTS; ++_ev.data.fd)
	{
		close(_ev.data.fd);
	}
	if (IPv4bind())
	{
		close(_fd_epoll);
		close(_fd_server);
	}
	std::cout << "Server resources cleaned up." << std::endl;
}
void Server::Start()
{
	// Start server implementation
}

int &Server::getServerFd()
{
	return _fd_server;
}
int &Server::getClientFd()
{
	return _fd_client;
}
String Server::getServerName()
{
	return _server_name;
}
void Server::Run()
{
	Server::_running = true;
	if (!createSocket())
	{
		throw std::runtime_error("Socket creation failed");
	}
	if (!socketUnblock(_fd_server))
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
}

bool Server::createSocket()
{
	// Crée la socket
	_fd_server = socket(_address.sin_family, SOCK_STREAM, 0);
	if (_fd_server < 0)
	{

		std::cerr << "Erreur création socket\n";
		return false;
	}

	// Configurer SO_REUSEADDR
	int opt = 1;
	if (setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Erreur setsockopt\n";
		close(getServerFd());
		return false;
	}
	return true;
}

bool Server::socketUnblock(int fd)
{
	// Set socket to non-blocking implementation
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return false;
	return true;
}

bool Server::IPv4bind()
{
	// IPv4 bind implementation
	if (bind(_fd_server, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		std::cerr << "Erreur bind\n";
		close(getServerFd());
		return false;
	}
	return true;
}
bool Server::listening()
{
	// Écoute des connexions entrantes
	if (listen(_fd_server, 10) < 0)
	{
		std::cerr << "Erreur listen\n";
		close(_fd_server);
		return false;
	}
	std::cout << "Serveur en écoute sur le port " << ntohs(_address.sin_port) << "...\n";
	return true;
}

bool Server::createPoll()
{
	// IPv4 bind implementation
	// equivalent de pool

	_fd_epoll = epoll_create1(0);
	if (_fd_epoll == -1)
	{
		std::cerr << "Erreur epoll_create1\n";
		close(getServerFd());
		return false;
	}
	return true;
}

bool Server::AddSocket()
{
	// Ajouter le socket serveur à epoll
	_ev.events = EPOLLIN;	  // Surveiller les événements de lecture
	_ev.data.fd = _fd_server; // Ajouter le socket serveur à epoll
	if (epoll_ctl(_fd_epoll, EPOLL_CTL_ADD, _fd_server, &_ev) == -1)
	{
		std::cerr << "Erreur epoll_ctl\n";
		close(_fd_server);
		close(_fd_epoll);
		return false;
	}
	return true;
}

bool Server::CheckPassword(String password, int fd)
{
	if (password == _password)
		return false;
	close(fd);
	return true;
}

Client *Server::findConnectedByfd(int idRecherche)
{
	std::map<int, Client *>::iterator it = _connected_clients.find(idRecherche);
	return (it != _connected_clients.end()) ? it->second : NULL;
}

Client *Server::findConnectedByNickname(String Nickname)
{
	for (std::map<int, Client *>::iterator it = _connected_clients.begin(); it != _connected_clients.end(); ++it)
	{
		if (it->second->getNickname() == Nickname)
			return it->second;
	}
	return (NULL);
}

Client *Server::findConnectedByUsername(String Find)
{
	for (std::map<int, Client *>::iterator it = _connected_clients.begin(); it != _connected_clients.end(); ++it)
	{
		if (it->second->getUsername() == Find)
			return it->second;
	}
	return (NULL);
}
void Server::Stop(int signum)
{
	(void)signum;
	std::cout << "\nStopping server..." << std::endl;
	Server::_running = false;
}

bool Server::CleanUp()
{
	// Nettoyage des ressources utilisées
	Server::_running = false;
	for (std::map<int, Client *>::iterator it = _connected_clients.begin(); it != _connected_clients.end(); ++it)
	{
		if (it->second)
			delete it->second;
	}
	_connected_clients.clear();
	if (!IPv4bind())
		return false;
	close(_ev.data.fd);
	close(_fd_epoll);
	close(getServerFd());
	std::cout << "Server cleaned up and closed." << std::endl;
	return true;
}

int Server::check_port(const char *port)
{
	int len = std::strlen(port);
	for (int i = 0; i < len; i++)
	{
		if (!isdigit(port[i]))
			throw std::invalid_argument("Invalid port number");
	}
	int port_num = std::atoi(port);
	if (port_num < 1 || port_num > 65535)
		throw std::invalid_argument("Invalid port number");
	return port_num;
}

bool Server::wait()
{
	// Boucle principale
	events->events = EPOLLIN | EPOLLET; // Edge-triggered
	Irc irc = Irc();
	while (Server::_running)
	{
		// Equivqalent de poll
		int nfds = epoll_wait(_fd_epoll, events, MAX_EVENTS, -1);
		if (nfds < 0)
		{
			if (_running == true)
				std::cerr << "Erreur epoll_wait\n";
			break;
		}
		// Traiter tous les événements
		for (int i = 0; i < nfds; i++)
		{
			// Nouvelle connexion sur le socket serveur
			if (events[i].data.fd == getServerFd())
			{
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int new_client_fd = accept(getServerFd(), (struct sockaddr *)&client_addr, &client_len);
				if (new_client_fd == -1)
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
				AddClient(new_client_fd, client_ip);

				// Rendre le socket client non-bloquant
				if (!socketUnblock(new_client_fd))
				{
					std::cerr << "Erreur socketUnblock client\n";
					close(new_client_fd);
					continue;
				}

				// Ajouter le client à epoll
				_ev.events = EPOLLIN | EPOLLOUT | EPOLLET; // Edge-triggered
				_ev.data.fd = new_client_fd;
				if (epoll_ctl(_fd_epoll, EPOLL_CTL_ADD, new_client_fd, &_ev) == -1)
				{
					std::cerr << "Erreur ajout client à epoll\n";
					close(new_client_fd);
				}
			}
			// Données disponibles sur un socket client
			else
			{
				int event_fd = events[i].data.fd;
				_fd_client = event_fd; // Sync member variable for methods using getClientFd()

				if (events[i].events & EPOLLIN)
				{
					char buffer[BUFFER_SIZE];
					
					ssize_t count = recv(event_fd, buffer, sizeof(buffer) - 1, 0);
					

					if (count == -1)
					{
						if (errno != EAGAIN)
						{
							std::cerr << "Erreur recv\n";
							epoll_ctl(_fd_epoll, EPOLL_CTL_DEL, event_fd, NULL);
							if (_connected_clients.find(event_fd) != _connected_clients.end())
							{
								irc.disconnectClient(_connected_clients[event_fd], "Connection reset by peer");
								delete _connected_clients[event_fd];
								_connected_clients.erase(event_fd);
							}
							close(event_fd);
						}
					}
					else if (count == 0)
					{
						// Client a fermé la connexion

						epoll_ctl(_fd_epoll, EPOLL_CTL_DEL, event_fd, NULL);
						if (_connected_clients.find(event_fd) != _connected_clients.end())
						{
							irc.disconnectClient(_connected_clients[event_fd], "Connection closed");
							delete _connected_clients[event_fd];
							_connected_clients.erase(event_fd);
						}
						close(event_fd);
					}
					else
					{
						// Traiter les données reçues
						buffer[count] = '\0';
						Client *client = findConnectedByfd(event_fd);
						if (client)
						{
							client->appendBuffer(buffer);
							std::string &clientBuffer = client->getBuffer();
							size_t pos;
							while ((pos = clientBuffer.find('\n')) != std::string::npos)
							{
								std::string line = clientBuffer.substr(0, pos + 1);
								irc.parseCommand(line, *this);
								clientBuffer.erase(0, pos + 1);
							}
							client->flush();
						}
					}
				}

				if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
				{
					epoll_ctl(_fd_epoll, EPOLL_CTL_DEL, event_fd, NULL);
					if (_connected_clients.find(event_fd) != _connected_clients.end())
					{
						irc.disconnectClient(_connected_clients[event_fd], "Unexpected disconnection");
						delete _connected_clients[event_fd];
						_connected_clients.erase(event_fd);
					}
					close(event_fd);
				}
				else if (events[i].events & EPOLLOUT)
				{
					Client *client = findConnectedByfd(event_fd);
					if (client)
						client->flush();
				}
			}
		}
		for (std::map<int, Client *>::iterator it = _connected_clients.begin(); it != _connected_clients.end(); ++it)
		{
			if (it->second)
				it->second->flush();
		}
	}
	std::cout << "Exiting main server loop." << std::endl;
	return true;
}

bool Server::AddClient(int fd, std::string ip)
{
	Client *newClient;
	newClient = new Client(fd, ip);
	_connected_clients[fd] = newClient;
	return newClient->isRegistered();
}

std::map<int, Client *> &Server::getConnectedClients()
{
	return _connected_clients;
}
