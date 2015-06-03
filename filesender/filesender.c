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
#include <bufio.h>

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: filesender port filename");
        return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res;
    if (getaddrinfo(NULL, argv[1], &hints, &res) != 0) {
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

    struct sockaddr_in client;
    socklen_t sz = sizeof(client);
    while (1) {
        int client_fd = accept(sfd, (struct sockaddr*)&client, &sz);
        if (client_fd == -1) {
            perror("accept");
            close(sfd);
            return -1;
        }

        pid_t pid = fork();
        if(pid < 0)
        {
            perror("fork");
            close(client_fd);
            close(sfd);
            return -1;
        }
        if(pid == 0) {
            close(sfd);
            int fd = open(argv[2], O_RDONLY);
            struct buf_t* buf = buf_new(4096);
            if(buf == NULL) {
                close(client_fd);
                close(fd);
                exit(1);
            }

            int r = 1, w = 1;
            while (1) {
                if (((r = buf_fill(fd, buf, buf_capacity(buf))) < 0) || w < 0) {
                    buf_free(buf);
                    close(client_fd);
                    close(fd);
                    exit(-1);
                }
                if(r == 0) {
                    buf_free(buf);
                    close(client_fd);
                    close(fd);
                    exit(0);
                }
                
                w = buf_flush(client_fd, buf, buf->size);
            }
        }
        close(client_fd);

    }
    close(sfd);
    return 0;
}
