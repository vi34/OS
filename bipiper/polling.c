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
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <signal.h>
#include "../lib/bufio.h"

#define MAX_CLIENTS_CNT 254
#define BUF_SIZE 4096

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

struct mypipe {
    struct buf_t* buffer1;
    struct buf_t* buffer2;
    int closing;
};

struct pollfd pollfds[2 + MAX_CLIENTS_CNT];
struct mypipe buffs[MAX_CLIENTS_CNT / 2];
int nfds;

void close_pipe(int pfd1, int pfd2, int buf_id) {
    buf_free(buffs[buf_id].buffer1);
    buf_free(buffs[buf_id].buffer2);
    close(pollfds[pfd1].fd);
    close(pollfds[pfd2].fd);
    pollfds[pfd1] = pollfds[nfds - 2 + (pfd1 % 2)];
    pollfds[pfd2] = pollfds[nfds - 2 + (pfd2 % 2)];
    nfds -= 2;
    int last_id = (nfds - 2) / 2;
    buffs[buf_id].buffer1 = buffs[last_id].buffer1;
    buffs[buf_id].buffer2 = buffs[last_id].buffer2;
}

int pipe_receive(int pfd1, int pfd2, int buf_id) {
    struct buf_t* buf;
    if (pfd1 % 2)
        buf = buffs[buf_id].buffer2;
    else
        buf = buffs[buf_id].buffer1;
    int prev_size = buf_size(buf);
    int r = buf_fill(pollfds[pfd1].fd, buf, prev_size + 1);
    if (r == prev_size) { // EOF
        pollfds[pfd1].events &= ~(POLLIN);
        if (buffs[buf_id].closing != -1) {
            close_pipe(pfd1, pfd2, buf_id);
        }
        buffs[buf_id].closing = !(pfd1 % 2);
    } else if(r < prev_size) { // error
        close_pipe(pfd1, pfd2, buf_id);
        return r;
    } else {
        if (buf_size(buf) == buf_capacity(buf)) {
            pollfds[pfd1].events &= ~(POLLIN);
        }
        pollfds[pfd2].events |= POLLOUT;
    }
    return 0;
}

int pipe_send(int pfd1, int pfd2, int buf_id) {
    struct buf_t* buf;
    if (pfd1 % 2)
        buf = buffs[buf_id].buffer1;
    else
        buf = buffs[buf_id].buffer2;
    int w = buf_flush(pollfds[pfd1].fd, buf, 1);
    if (w < 0) {
        close_pipe(pfd1, pfd2, buf_id);
        return -1;
    }
    if (buf_size(buf) == 0) {
        pollfds[pfd1].events &= ~(POLLOUT);
        if(buffs[buf_id].closing == (pfd1 % 2)) {

            shutdown(pollfds[pfd1].fd, SHUT_WR);
        }
    }

    if (buf_size(buf) < buf_capacity(buf))
        pollfds[pfd2].events |= POLLIN;

    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: polling port1 port2");
        return 1;
    }

    sfd = bind_port(argv[1]);
    sfd2 = bind_port(argv[2]);
    pollfds[0].fd = sfd;
    pollfds[1].fd = sfd2;
    pollfds[0].events = POLLIN;
    nfds = 2;
    int state = 0;
    while (1) {
        int cnt = poll(pollfds, nfds, 10000);
        if (cnt < 0) {
            perror("poll");
            if (errno != EINTR)
                exit(1);
            continue;
        } else if (cnt > 0) {
            for (int i = 0; i < nfds; ++i) {
                if(pollfds[i].revents != 0) {
                    if (i == state && (nfds < 2 + MAX_CLIENTS_CNT)) {
                        int clientfd = accept_client(pollfds[state].fd);
                        fprintf(stderr, "accept client %d\n", clientfd);
                        pollfds[nfds + state].fd = clientfd;
                        pollfds[nfds + state].events = POLLIN;
                        pollfds[state].events = 0;
                        pollfds[!state].events = POLLIN;
                        if(state == 1) {
                            buffs[(nfds - 2) / 2].buffer1 = buf_new(BUF_SIZE);
                            buffs[(nfds - 2) / 2].buffer2 = buf_new(BUF_SIZE);
                            buffs[(nfds - 2) / 2].closing = -1;
                            nfds += 2;
                        }

                        state = !state;
                    } else if(i > 1) {
                        int buf_id = (i - (i % 2) - 2) / 2;
                        int pair = ((i % 2) ? i - 1 : i + 1);
                        if(pollfds[i].revents & POLLIN) {
                            if (pipe_receive(i, pair, buf_id) < 0 ) {
                                fprintf(stderr, "error while receiving, some clients had closed");
                            }
                        } else if (pollfds[i].revents & POLLOUT) {
                            if (pipe_send(i, pair, buf_id) < 0) {
                                fprintf(stderr, "error while sending, some clients had closed");
                            }
                        } else {
                            fprintf(stderr, "some error occured");
                        }
                    }
                }
            }
        }
    }
    close(sfd);
    close(sfd2);
    return 0;
}
