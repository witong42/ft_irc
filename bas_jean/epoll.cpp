while (true)
{
    // 1. The ONLY blocking point
    int n = epoll_wait(epfd, events, 10, -1);

    for (int i = 0; i < n; i++)
    {
        int current_fd = events[i].data.fd;

        // CASE A: Reading
        if (events[i].events & EPOLLIN)
        {
            recv(current_fd, buffer, ...);

            // Prepare response, but DO NOT SEND yet.
            client_buffers[current_fd] = "Hello World";

            // Modify epoll to listen for EPOLLOUT (Writing)
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLOUT; // Monitor Read AND Write
            ev.data.fd = current_fd;
            epoll_ctl(epfd, EPOLL_CTL_MOD, current_fd, &ev);
        }

        // CASE B: Writing
        // This is the ONLY place send() exists.
        // It is impossible to reach this line without passing epoll_wait first.
        else if (events[i].events & EPOLLOUT)
        {

            send(current_fd, client_buffers[current_fd], ...);

            // Once done sending, remove EPOLLOUT flag so we don't spin loop
            struct epoll_event ev;
            ev.events = EPOLLIN; // Go back to only listening for Read
            ev.data.fd = current_fd;
            epoll_ctl(epfd, EPOLL_CTL_MOD, current_fd, &ev);
        }
    }
}