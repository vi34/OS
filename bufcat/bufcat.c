#include "../lib/bufio.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const int BUF_SIZE = 512;

int main() {
    struct buf_t* buf = buf_new(BUF_SIZE);
    int r,w;
    while((r = buf_fill(STDIN_FILENO, buf, buf_capacity(buf))) >= buf_capacity(buf)) {
        w = buf_flush(STDOUT_FILENO, buf, buf_size(buf));
        if(w == -1){
            perror("buf_flush");
            exit(1);
        }
    }
    w = buf_flush(STDOUT_FILENO,buf, buf_size(buf));
    if(r == -1 || w == -1) {
        exit(1);
    }
    buf_free(buf);
    return 0;
    
}
