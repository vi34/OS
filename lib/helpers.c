#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "helpers.h"

ssize_t read_(int fildes, void *buf, size_t nbyte) {
    read(fildes,buf,nbyte);
    return 0;
}

ssize_t write_(int fildes, const void *buf, size_t nbyte) {
    write(fildes, buf, nbyte);
    return 0;
}