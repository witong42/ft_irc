#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <map>
#include <string>
#include <cstring>

#define MAX_EVENTS 1024
#define BUFFER_SIZE 512
#define PORT 6667

int main()
{
	int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);
	bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
	listen(server_fd, SOMAXCONN);

	int epfd = epoll_create1(EPOLL_CLOEXEC);
	struct epoll_event ev = {0};
	ev.events = EPOLLIN;
	ev.data.fd = server_fd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

	struct epoll_event events[MAX_EVENTS];
	std::map<int, std::string> out_queues;
	char buffer[BUFFER_SIZE];

	while (true)
	{
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (nfds <= 0)
			continue;

		for (int i = 0; i < nfds; ++i)
		{
			int fd = events[i].data.fd;

			// ACCEPT seulement après epoll_wait + EPOLLIN sur server
			if (fd == server_fd && (events[i].events & EPOLLIN))
			{
				int client_fd;
				while ((client_fd = accept(server_fd, NULL, NULL)) > 0)
				{
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = client_fd;
					epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
				}
				continue;
			}

			// RECV seulement après epoll_wait + EPOLLIN
			if ((events[i].events & EPOLLIN))
			{
				ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
				if (n > 0)
				{
					// Echo/response IRC : queue pour send
					std::string reply = "REPLY: ";
					reply.append(buffer, n);
					out_queues[fd] += reply;

					// Active OUT si besoin
					ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
					ev.data.fd = fd;
					epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
				}
				else
				{
					// Fin ou erreur : close (sans check errno)
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
					close(fd);
					out_queues.erase(fd);
				}
				continue;
			}

			// SEND seulement après epoll_wait + EPOLLOUT
			if ((events[i].events & EPOLLOUT) && !out_queues[fd].empty())
			{
				std::string &out = out_queues[fd];
				ssize_t sent = send(fd, out.data(), out.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
				if (sent > 0)
				{
					out.erase(0, sent);
				}
				if (out.empty())
				{
					ev.events = EPOLLIN | EPOLLET; // Back to IN only
					ev.data.fd = fd;
					epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
				}
				// PAS D'ACTION SUR EAGAIN : epoll_wait suivant gèrera
			}

			if (events[i].events & (EPOLLHUP | EPOLLERR))
			{
				epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
				out_queues.erase(fd);
			}
		}
	}
	return 0;
}
