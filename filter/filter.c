#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const size_t SIZE = 4096;

int main(int argc, char* argv[]) {
    char buf[SIZE];
    char word[SIZE + 1];
    int r, w;
    int index = 0;

    for(int i = 0; i < argc - 1; ++i)
    {
        argv[i] = argv[i + 1];
    }
    argv[argc - 1] = word;

    while(1)
    {
        r = read_until(STDIN_FILENO, buf, SIZE, '\n');
        if (r == 0)
            break;
        if (r < 0) {
            perror("read");
            exit(1);
        }
        for(int i = 0; i < r; ++i)
        {
            if(((char*)buf)[i] == '\n')
            {
                word[index] = 0;
                int sp_res = spawn(argv[0], argv);
                if(sp_res == 0)
                {
                    word[index++] = '\n';
                    w = write_(STDOUT_FILENO, word, index);
                    if(w < 0)
                    {
                        perror("write");
                        exit(1);
                    }
                }
                index = 0;
            } else {
                word[index++] = buf[i];
            }
        }
    }
    if(index > 0)
    {
        word[index] = 0;
        argv[argc - 1] = word;
        int sp_res = spawn(argv[0], argv);
        if(sp_res == 0)
        {
            word[index++] = '\n';
            w = write_(STDOUT_FILENO, word, index);
            if(w < 0)
            {
                perror("write");
                exit(1);
            }
        }
    }

    return 0;
}
