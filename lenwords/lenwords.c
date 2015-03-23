#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const size_t SIZE = 4096;

int main() {
    char buf[SIZE];
    char num [10];
    int r, w;
    int index = 0;
    int digits;

    while(1)
    {
        r = read_until(STDIN_FILENO, buf, SIZE, ' ');
        if (r == 0)
            break;
        if (r < 0) {
            perror("read");
            exit(1);
        }
        for(int i = 0; i < r; ++i)
        {
            if(((char*)buf)[i] == ' ')
            {
                digits = sprintf(num,"%d ",index);
                w = write_(STDOUT_FILENO,num, digits);
                if(w < 0)
                {
                    perror("write");
                    exit(1);
                }
                index = 0;
            } else {
                index++;
            }
        }
    }
    if(index > 0)
    {
        digits = sprintf(num,"%d ",index);
        w = write_(STDOUT_FILENO,num, digits);
        if(w < 0)
        {
            perror("write");
            exit(1);
        }

    }

    return 0;
}