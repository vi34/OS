#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const int BUF_SIZE = 512;

int main() {
    char buf[BUF_SIZE];
    int r,w;

    while((r = read_(STDIN_FILENO,buf,BUF_SIZE)) > 0) {
        w = write_(STDOUT_FILENO,buf,r);
        if(w < 0) {
            perror("write");
            exit(1);
        }
        memset(buf,0,BUF_SIZE);
    }
    if(r < 0) {
        perror("read");
        exit(1);
    }
    return 0;
}