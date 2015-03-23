#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const size_t SIZE = 4096;

void reverse(char* word, int index) {
    for(int j = 0; j < index / 2; ++j)
    {
        int tmp = word[j];
        word[j] = word[index - j - 1];
        word[index - j - 1] = tmp;
    }
}

int main() {
    char buf[SIZE];
    char word[SIZE + 1];
    int r, w;
    int index = 0;

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
                reverse(word, index);
                word[index++] = ' ';
                w = write_(STDOUT_FILENO,word, index);
                if(w < 0)
                {
                    perror("write");
                    exit(1);
                }
                index = 0;
            } else {
                word[index++] = buf[i];
            }
        }
    }
    if(index > 0)
    {
        reverse(word, index);
        w = write_(STDOUT_FILENO,word, index);
        if(w < 0)
        {
            perror("write");
            exit(1);
        }
    }

    return 0;
}