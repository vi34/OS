#ifndef _HELPERS_H
#define _HELPERS_H

#include <unistd.h>

ssize_t read_(int fildes, void *buf, size_t nbyte);
ssize_t write_(int fildes, const void* buf, size_t nbyte);

#endif