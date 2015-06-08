#define _POSIX_SOURCE

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../lib/bufio.h"

int bind_port(char* port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        return -1;
    }

    struct addrinfo* rp;
    int sfd;
    for(rp = res; rp != NULL; rp = rp->ai_next ) {
        if ((sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) < 0)
            continue;
        int one = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1)
            perror("setsockopt");
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(sfd);
    }

    if(rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(sfd, 10) == -1)
        perror("listen");

    return sfd;
}

int accept_client(int sfd) {
    struct sockaddr_in client;
    socklen_t sz = sizeof(client);
    int fd = accept(sfd, (struct sockaddr*)&client, &sz);
    if (fd == -1) {
        perror("accept");
        close(sfd);
        return -1;
    }
    return fd;
}

int sfd;
int sfd2;

pid_t make_child(int fd_from, int fd_to) {
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
        close(fd_from);
        close(fd_to);
        return -1;
    }
    if(pid == 0) {
        struct buf_t* buf = buf_new(4096);
        if(buf == NULL) {
            shutdown(fd_to, SHUT_WR);
            close(fd_from);
            close(fd_to);
            close(sfd);
            close(sfd2);
            exit(1);
        }

        int r = 1, w = 1;
        while (1) {
            if (((r = buf_fill(fd_from, buf, 1)) < 0) || w < 0) {
                buf_free(buf);
                close(fd_from);
                close(fd_to);
                close(sfd);
                close(sfd2);
                exit(1);
            }

            if(r == 0) {
                buf_free(buf);
                close(fd_from);
                close(fd_to);
                close(sfd);
                close(sfd2);
                exit(0);
            }
            w = buf_flush(fd_to, buf, buf->size);
        }
    }
    return pid;
}



int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: forking port1 port2");
        return 1;
    }

    sfd = bind_port(argv[1]);
    sfd2 = bind_port(argv[2]);
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGCHLD, &sa, NULL);

    while (1) {
        int client1 = accept_client(sfd);
        int client2 = accept_client(sfd2);
        if(client1 == -1 || client2 == -1) {
            return -1;
        }

        pid_t pid1;
        if ((pid1 = make_child(client1, client2)) < 0 || make_child(client2, client1) < 0) {
            close(sfd);
            close(sfd2);
            if(pid1 > 0)
                kill(pid1, SIGTERM);
            exit(0);
        }
        close(client1);
        close(client2);
    }
    close(sfd);
    close(sfd2);
    return 0;
}
