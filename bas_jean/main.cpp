#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

// Fonction pour mettre une socket en mode non-bloquant
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    // 1. Créer la socket serveur
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Erreur création socket" << std::endl;
        return 1;
    }

    // 2. Configurer SO_REUSEADDR
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Mettre la socket en non-bloquant
    if (set_nonblocking(server_fd) < 0) {
        std::cerr << "Erreur set_nonblocking" << std::endl;
        close(server_fd);
        return 1;
    }

    // 4. Bind
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Erreur bind" << std::endl;
        close(server_fd);
        return 1;
    }

    // 5. Listen
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Erreur listen" << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout << "Serveur en écoute sur port 8080..." << std::endl;

    // 6. Créer l'instance epoll
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        std::cerr << "Erreur epoll_create1" << std::endl;
        close(server_fd);
        return 1;
    }

    // 7. Ajouter le server_fd à epoll
    struct epoll_event ev;
    ev.events = EPOLLIN; // Surveiller les événements de lecture
    ev.data.fd = server_fd;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
        std::cerr << "Erreur epoll_ctl" << std::endl;
        close(server_fd);
        close(epoll_fd);
        return 1;
    }

    // 8. Buffer pour les événements
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];

    std::cout << "En attente de connexions (Ctrl+C pour arrêter)..." << std::endl;

    // 9. Boucle principale
    while (true) {
        // Attendre des événements (timeout -1 = infini)
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        
        if (nfds < 0) {
            std::cerr << "Erreur epoll_wait" << std::endl;
            break;
        }

        // 10. Traiter chaque événement
        for (int i = 0; i < nfds; i++) {
            // Si c'est la socket serveur = nouvelle connexion
            if (events[i].data.fd == server_fd) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd < 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        std::cerr << "Erreur accept" << std::endl;
                    }
                    continue;
                }

                std::cout << "Nouveau client connecté: fd=" << client_fd << std::endl;

                // Mettre le client en non-bloquant
                set_nonblocking(client_fd);

                // Ajouter le client à epoll
                ev.events = EPOLLIN | EPOLLET; // Edge-triggered
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
                    std::cerr << "Erreur ajout client à epoll" << std::endl;
                    close(client_fd);
                }
            }
            // Sinon c'est un client qui a des données
            else {
                int client_fd = events[i].data.fd;
                char buffer[1024];
                
                ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    std::cout << "Reçu de fd=" << client_fd << ": " << buffer;
                    
                    // Echo: renvoyer le message
                    send(client_fd, buffer, bytes_read, 0);
                }
                else if (bytes_read == 0) {
                    // Client déconnecté
                    std::cout << "Client déconnecté: fd=" << client_fd << std::endl;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    close(client_fd);
                }
                else {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        std::cerr << "Erreur recv" << std::endl;
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                        close(client_fd);
                    }
                }
            }
        }
    }

    // Nettoyage
    close(server_fd);
    close(epoll_fd);
    return 0;
}