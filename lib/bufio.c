#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bufio.h"

void assertm(int cond) {
#ifdef DEBUG
    if(!cond) {
        abort();
    }
#endif
}

struct buf_t *buf_new(size_t capacity) {
    struct buf_t* p;
    if((p = (struct buf_t *)malloc(sizeof(struct buf_t) + capacity)) == NULL) {
        return NULL;
    }
    p->size = 0;
    p->capacity = capacity;
    return p;
}

void buf_free(struct buf_t *buf) {
    assertm(buf != NULL);
    free(buf);
}

size_t buf_capacity(struct buf_t *buf) {
    assertm(buf != NULL);
    return buf->capacity;
}

size_t buf_size(struct buf_t *buf) {
    assertm(buf != NULL);
    return buf->size;
}

ssize_t buf_fill(int fd,struct buf_t *buf, size_t required) {
    assertm(buf != NULL);
    assertm(buf->capacity < required);

    ssize_t r;
    while(buf->size < required) {
        r = read(fd, buf + buf->size, buf->capacity - buf->size);
        if (r < 0) {
            return -1;
        }
        if (r == 0) {
            break;
        }
        buf->size += r;
    }

    return buf->size;
}

ssize_t buf_flush(int fd,struct buf_t *buf, size_t required) {
    assertm(buf != NULL);
    ssize_t w;
    size_t count = 0;

    if(required > buf->size)
        required = buf->size;

    while (count < required) {
        w = write(fd, buf->buffer + count, required - count);
        if(w < 0) {
            memmove(buf->buffer, buf->buffer + count, buf->size - count);
            return -1;
        }
        count += w;
    }
    memmove(buf->buffer,buf->buffer + count, buf->size - count);
    buf->size -= count;
    return count;
}