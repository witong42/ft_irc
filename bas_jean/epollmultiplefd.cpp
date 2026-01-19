/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epollmultiplefd.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 12:09:13 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/19 13:01:54 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <errno.h>
#include <iostream>

#define MAX_EVENTS 100
#define BUFFER_SIZE 512
#define PORT 6667

int main()
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		return 1;

	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	fcntl(server_fd, F_SETFL, O_NONBLOCK); // Non-bloquant

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);
	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		return 1;
	if (listen(server_fd, SOMAXCONN) < 0)
		return 1;

	int epfd = epoll_create1(0);
	if (epfd < 0)
		return 1;

	struct epoll_event ev = {};
	ev.events = EPOLLIN;
	ev.data.fd = server_fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev) < 0)
		return 1;

	struct epoll_event events[MAX_EVENTS];
	char buffer[BUFFER_SIZE];
	while (true)
	{
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (nfds < 0)
			continue;

		for (int i = 0; i < nfds; ++i)
		{
			int fd = events[i].data.fd;
			if (fd == server_fd && (events[i].events & EPOLLIN))
			{
				// Accept
				int client_fd;
				while ((client_fd = accept(server_fd, NULL, NULL)) >= 0)
				{
					fcntl(client_fd, F_SETFL, O_NONBLOCK);
					ev.events = EPOLLIN | EPOLLET; // Edge-triggered
					ev.data.fd = client_fd;
					epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
					// Welcome IRC: send(client_fd, "001 Welcome\r\n", 12, 0);
				}
			}
			else if (events[i].events & EPOLLIN)
			{
				// Recv
				ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
				if (n <= 0)
				{
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
					close(fd);
					continue;
				}
				buffer[n] = '\0';
				// Broadcast à tous (simplifié ; utilisez map<fd, state> pour outbuf)
				// Pour vrai IRC, queue par client et EPOLLOUT si outbuf non vide
			}
			if (events[i].events & EPOLLOUT)
			{
				std::cout << "EPOLLOUT for fd: " << fd << std::endl;
				
				// Send pending data (ex. : buffers out) ; ignore EAGAIN
			}
			if (events[i].events & (EPOLLERR | EPOLLHUP))
			{
				epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
			}
		}
	}
	close(epfd);
	close(server_fd);
	return 0;
}
