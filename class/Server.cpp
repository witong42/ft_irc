/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:18 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/15 17:39:31 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>

#include <iostream>
#include "server.hpp"
#include <cctype>
#include <cstring>
#include <stdexcept>


bool Server::socket()
{
    // Socket creation implementation
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0)
        return false;
	
		int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		return false;
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

bool Server::IPv4 bind()
{
	std::cout << "Serveur en écoute sur le port " << PORT << "...\n";
	// IPv4 bind implementation
int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "Erreur epoll_create1\n";
        close(server_fd);
        return 1;
    }
}



tmp{
 // 5. Ajouter le socket serveur à epoll
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;  // Surveiller les événements de lecture
    ev.data.fd = server_fd;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        std::cerr << "Erreur epoll_ctl\n";
        close(server_fd);
        close(epoll_fd);
        return 1;
    }

    // 6. Boucle principale
    while (true) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            std::cerr << "Erreur epoll_wait\n";
            break;
        }

        // Traiter tous les événements
        for (int i = 0; i < nfds; i++) {
            // Nouvelle connexion sur le socket serveur
            if (events[i].data.fd == server_fd) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        std::cerr << "Erreur accept\n";
                    }
                    continue;
                }

                // Afficher info client
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                std::cout << "Nouvelle connexion de " << client_ip 
                          << ":" << ntohs(client_addr.sin_port) << "\n";

                // Rendre le socket client non-bloquant
                set_nonblocking(client_fd);

                // Ajouter le client à epoll
                ev.events = EPOLLIN | EPOLLET;  // Edge-triggered
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                    std::cerr << "Erreur ajout client à epoll\n";
                    close(client_fd);
                }
            }
            // Données disponibles sur un socket client
            else {
                int client_fd = events[i].data.fd;
                char buffer[BUFFER_SIZE];
                
                ssize_t count = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                
                if (count == -1) {
                    if (errno != EAGAIN) {
                        std::cerr << "Erreur recv\n";
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                        close(client_fd);
                    }
                }
                else if (count == 0) {
                    // Client a fermé la connexion
                    std::cout << "Client déconnecté (fd: " << client_fd << ")\n";
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    close(client_fd);
                }
                else {
                    // Afficher les données reçues
                    buffer[count] = '\0';
                    std::cout << "Reçu (" << count << " octets): " << buffer;
                    
                    // Echo - renvoyer les données au client
                    send(client_fd, buffer, count, 0);
                }
            }
        }
    }

    // Nettoyage
    close(server_fd);
    close(epoll_fd);
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