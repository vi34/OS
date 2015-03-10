#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int main() {
    char buf[10];
    read_(0,buf,10);
    write_(1,buf,10);
    return 0;
}