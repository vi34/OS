#ifndef _HELPERS_H
#define _HELPERS_H

#include <unistd.h>

ssize_t read_(int fildes, void *buf, size_t nbyte);
ssize_t write_(int fildes, const void* buf, size_t nbyte);
ssize_t read_until(int fd, void * buf, size_t count, char delimiter);
int spawn(const char* file, char* const argv[]);


#define MAX_ARGS_CNT 100
struct execargs_t {
    char* file;
    char* args[MAX_ARGS_CNT];
    int argc;
};

struct execargs_t* execargs_new();
int exec(struct execargs_t* args);
int runpiped(struct execargs_t** programs, size_t n);


#endif
