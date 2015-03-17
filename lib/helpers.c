#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
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