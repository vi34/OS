#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "helpers.h"

ssize_t read_(int fildes, void *buf, size_t nbyte) {
    int n;
    ssize_t count = 0;
    while ((n = read(fildes,buf + count,nbyte - count)) > 0 && count < nbyte) {
        count += n;
    }
    if(n < 0)
        return n;
    return count;
}

ssize_t write_(int fildes, const void *buf, size_t nbyte) {
    int n;
    ssize_t count = 0;
    while((n = write(fildes, buf + count, nbyte - count)) >= 0 && count < nbyte) {
        count += n;
    }
    if(n < 0)
        return n;
    return count;
}