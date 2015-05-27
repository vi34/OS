#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

ssize_t read_(int fd, void *buf, size_t nbyte) {
    int n;
    ssize_t count = 0;
    while ((n = read(fd,buf + count,nbyte - count)) > 0 && count < nbyte) {
        count += n;
    }
    if(n < 0)
        return n;
    return count;
}

ssize_t write_(int fd, const void *buf, size_t nbyte) {
    int n;
    ssize_t count = 0;
    while((n = write(fd, buf + count, nbyte - count)) >= 0 && count < nbyte) {
        count += n;
    }
    if(n < 0)
        return n;
    return count;
}

ssize_t read_until(int fd, void * buf, size_t nbyte, char delimiter) {
    int n;
    ssize_t count = 0;

    while ((n = read(fd,buf + count,nbyte - count)) > 0 && count < nbyte) {
        count += n;
        int b = 0;
        for(int i = 0; i < n && !b; ++i)
            if(((char*)buf)[i + count - n] == delimiter)
                b = 1;
        if (b)
            break;
    }
    if(n < 0)
        return n;
    return count;
}

int spawn(const char * file, char * const argv [])
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return -1;
    } else if (pid == 0) { //child
        execvp(file,argv);
        perror("exec");
        return -1;
    } else {
        int status;
        if(wait(&status) < 0)
        {
            perror("wait");
            return -1;
        }
        return status;
    }
    return 0;
}

struct execargs_t* execargs_new() {
    struct execargs_t* p;
    p = (struct execargs_t*) malloc(sizeof(struct execargs_t));
    return p;
}

int exec(struct execargs_t* args) {
    return 0;
}

int runpiped(struct execargs_t** programs, size_t n) {
    for(int i = 0; i < n; ++i)
    {
        printf("run %s\n",programs[i]->file);
    }
    return 0;
}
