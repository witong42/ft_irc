/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: witong <witong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:18 by jegirard          #+#    #+#             */
/*   Updated: 2026/02/02 11:53:18 by witong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <iostream>
#include <string>

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
#include "../header/Logger.hpp"

// /connect localhost 6667 pwd123

bool Server::_running = true;

Server::Server(const char *port_char, String password) : _fd_server(-1), _fd_epoll(-1), _fd_client(-1), _password(password)
{
	int port = check_port(port_char);
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_port = htons(port);
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	char hostname[256];
	gethostname(hostname, sizeof(hostname));
	_server_name = String(hostname);
	Logger::info("Server name: " + _server_name);
}

Server::~Server()
{
	Logger::info("Cleaning up server resources...");

	for (std::map<int, Client *>::iterator it = _connected_clients.begin(); it != _connected_clients.end(); ++it)
	{
		if (it->second)
			delete it->second;
	}
	_connected_clients.clear();

	if (_fd_epoll >= 0)
		close(_fd_epoll);
	if (_fd_server >= 0)
		close(_fd_server);

	_fd_epoll = -1;
	_fd_server = -1;

	Logger::info("Server resources cleaned up.");
}

void Server::Run()
{
	Server::_running = true;

	if (!createSocket())
		throw std::runtime_error("Socket creation failed");
	if (!socketUnblock(_fd_server))
		throw std::runtime_error("Setting socket to non-blocking failed");
	if (!IPv4bind())
		throw std::runtime_error("Binding socket failed");
	if (!listening())
		throw std::runtime_error("Listening on socket failed");
	if (!createPoll())
		throw std::runtime_error("Creating epoll instance failed");
	if (!addSocket())
		throw std::runtime_error("Adding socket to epoll failed");
	if (!wait())
		throw std::runtime_error("Server wait loop failed");
}

void Server::Stop(int signum)
{
	(void)signum;
	Logger::info("Stopping server...");
	Server::_running = false;
}

bool Server::CleanUp()
{
	Server::_running = false;

	for (std::map<int, Client *>::iterator it = _connected_clients.begin(); it != _connected_clients.end(); ++it)
	{
		if (it->second)
			delete it->second;
	}
	_connected_clients.clear();

	if (_fd_epoll >= 0)
		close(_fd_epoll);
	if (_fd_server >= 0)
		close(_fd_server);

	_fd_epoll = -1;
	_fd_server = -1;

	Logger::info("Server cleaned up and closed.");
	return true;
}

int Server::check_port(const char *port)
{
	int len = std::strlen(port);
	for (int i = 0; i < len; ++i)
	{
		if (!isdigit(port[i]))
			throw std::invalid_argument("Invalid port number");
	}
	int port_num = std::atoi(port);
	if (port_num < 1 || port_num > 65535)
		throw std::invalid_argument("Invalid port number");
	return port_num;
}

bool Server::createSocket()
{
	_fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd_server < 0)
	{
		std::cerr << "Error socket creation\n";
		_fd_server = -1;
		return false;
	}

	int opt = 1;
	if (setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error setsockopt\n";
		close(_fd_server);
		_fd_server = -1;
		return false;
	}
	return true;
}

// bool Server::socketUnblock(int fd)
// {
// 	int flags = fcntl(fd, F_GETFL, 0);
// 	if (flags == -1)
// 		return false;
// 	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
// 		return false;
// 	return true;
// }

bool Server::socketUnblock(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		return false;
	return true;
}

bool Server::IPv4bind()
{
	if (bind(_fd_server, reinterpret_cast<struct sockaddr *>(&_address), sizeof(_address)) < 0)
	{
		std::cerr << "Error bind\n";
		close(_fd_server);
		_fd_server = -1;
		return false;
	}
	return true;
}

bool Server::listening()
{
	if (listen(_fd_server, 10) < 0)
	{
		std::cerr << "Error listen\n";
		close(_fd_server);
		_fd_server = -1;
		return false;
	}
	std::stringstream ss;
	ss << "Server listening on port " << ntohs(_address.sin_port) << "...";
	Logger::info(ss.str());
	return true;
}

bool Server::createPoll()
{
	_fd_epoll = epoll_create1(0);
	if (_fd_epoll == -1)
	{
		std::cerr << "Error epoll_create1\n";
		if (_fd_server >= 0)
		{
			close(_fd_server);
			_fd_server = -1;
		}
		return false;
	}
	return true;
}

bool Server::addSocket()
{
	_ev.events = EPOLLIN;
	_ev.data.fd = _fd_server;
	if (epoll_ctl(_fd_epoll, EPOLL_CTL_ADD, _fd_server, &_ev) == -1)
	{
		std::cerr << "Error epoll_ctl\n";
		if (_fd_server >= 0)
		{
			close(_fd_server);
			_fd_server = -1;
		}
		if (_fd_epoll >= 0)
		{
			close(_fd_epoll);
			_fd_epoll = -1;
		}
		return false;
	}
	return true;
}

bool Server::addClient(int fd, std::string ip)
{
	Client *newClient = new Client(fd, ip);
	_connected_clients[fd] = newClient;
	return newClient->isRegistered();
}

void Server::serverDisconnectClient(int fd, Irc &irc, std::string reason)
{
	if (_fd_epoll >= 0)
		epoll_ctl(_fd_epoll, EPOLL_CTL_DEL, fd, NULL);

	std::map<int, Client *>::iterator it = _connected_clients.find(fd);
	if (it != _connected_clients.end())
	{
		irc.ircDisconnectClient(it->second, reason);
		delete it->second;
		_connected_clients.erase(it);
	}
	else
	{
		if (fd >= 0)
			close(fd);
	}
}

bool Server::wait()
{
	Irc irc;
	while (Server::_running)
	{
		int nfds = epoll_wait(_fd_epoll, events, MAX_EVENTS, -1);
		if (nfds < 0)
		{
			if (_running)
				std::cerr << "Error epoll_wait\n";
			break;
		}

		for (int i = 0; i < nfds; ++i)
		{
			struct epoll_event &ev = events[i];

			if (ev.data.fd == _fd_server)
			{
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int new_client_fd = accept(_fd_server, reinterpret_cast<struct sockaddr *>(&client_addr), &client_len);
				if (new_client_fd == -1)
				{
					if (errno != EAGAIN && errno != EWOULDBLOCK)
						std::cerr << "Error accept\n";
					continue;
				}

				char client_ip[INET_ADDRSTRLEN];
				inet_ntop(_address.sin_family, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
				addClient(new_client_fd, client_ip);

				if (!socketUnblock(new_client_fd))
				{
					std::cerr << "Error socketUnblock client\n";
					close(new_client_fd);
					continue;
				}

				_ev.events = EPOLLIN;
				_ev.data.fd = new_client_fd;
				if (epoll_ctl(_fd_epoll, EPOLL_CTL_ADD, new_client_fd, &_ev) == -1)
				{
					std::cerr << "Error add client to epoll\n";
					close(new_client_fd);
				}
			}
			else
			{
				int event_fd = ev.data.fd;
				_fd_client = event_fd;

				if (ev.events & EPOLLIN)
				{
					char buffer[BUFFER_SIZE];
					// Read only once per event
					ssize_t count = recv(event_fd, buffer, sizeof(buffer) - 1, 0);
					if (count == -1)
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
						{
							// no data available right now; just continue
							continue;
						}
						std::cerr << "Error recv" << std::endl;
						serverDisconnectClient(event_fd, irc, "Connection error");
					}
					else if (count == 0)
					{
						serverDisconnectClient(event_fd, irc, "Connection closed");
					}
					else
					{
						buffer[count] = '\0';
						Client *client = findConnectedByfd(event_fd);
						if (client)
						{
							client->appendReadBuffer(buffer);
							std::string &clientBuffer = client->getReadBuffer();
							Logger::debug("Client buffer :" + client->getReadBuffer());

							// To refactor
							if (clientBuffer.size() > BUFFER_SIZE)
							{
								clientBuffer.erase(0, clientBuffer.size() - BUFFER_SIZE);
								std::cerr << "Read buffer truncated for fd " << event_fd << std::endl;
							}

							size_t pos;
							while ((pos = clientBuffer.find("\r\n")) != std::string::npos)
							{
								std::string line = clientBuffer.substr(0, pos);
								irc.switchCommand(line, *this);
								if (findConnectedByfd(event_fd) == NULL)
									break;
								clientBuffer.erase(0, pos + 2);
							}
						}
					}
					// EPOLLOUT for all clients with pending writes
					for (std::map<int, Client *>::iterator it = _connected_clients.begin(); it != _connected_clients.end(); ++it)
					{
						if (it->second->hasPendingWrites())
						{
							_ev.events = EPOLLIN | EPOLLOUT;
							_ev.data.fd = it->second->getFd();
							epoll_ctl(_fd_epoll, EPOLL_CTL_MOD, it->second->getFd(), &_ev);
						}
					}
				}

				if (ev.events & EPOLLOUT)
				{
					Client *client = findConnectedByfd(event_fd);
					if (client)
					{
						client->flush();
						// If done writing, disable EPOLLOUT
						if (!client->hasPendingWrites())
						{
							_ev.events = EPOLLIN;
							_ev.data.fd = event_fd;
							epoll_ctl(_fd_epoll, EPOLL_CTL_MOD, event_fd, &_ev);

							if (client->getShouldDisconnect())
								serverDisconnectClient(event_fd, irc, "Disconnecting after flush");
						}
					}
				}

				if (ev.events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
				{
					serverDisconnectClient(event_fd, irc, "Unexpected disconnection");
				}
			}
		}
	}

	std::cout << "Exiting main server loop." << std::endl;
	return true;
}

int &Server::getServerFd()
{
	return _fd_server;
}

bool Server::checkPassword(String password, Client *client)
{
	if (!client)
		return false;

	if (password == _password)
	{
		client->setPwdOk(true);
		return true;
	}
	return false;
}

Client *Server::findConnectedByfd(int idRecherche)
{
	std::map<int, Client *>::iterator it = _connected_clients.find(idRecherche);
	return (it != _connected_clients.end()) ? it->second : NULL;
}

std::map<int, Client *> &Server::getConnectedClients()
{
	return _connected_clients;
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

int &Server::getClientFd()
{
	return _fd_client;
}

String Server::getServerName()
{
	return _server_name;
}
