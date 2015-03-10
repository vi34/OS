#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


int main() {
    char buf[300];
    int k;
    memset(buf,0,300);
    k = read_(STDIN_FILENO,buf,300);
    //printf("\r\n%d",k);
    write_(STDOUT_FILENO,buf,300);
    return 0;
}