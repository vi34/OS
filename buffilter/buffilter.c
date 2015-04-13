#include "../lib/bufio.h"
#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const size_t SIZE = 4096;

int main(int argc, char* argv[]) {
    struct buf_t *buf = buf_new(SIZE);
    struct buf_t *buf2 = buf_new(SIZE);
    if(buf == NULL || buf2 == NULL)
        return -1;
    char word[SIZE + 1];
    int r, w;
    int index = 0;

    for(int i = 0; i < argc - 1; ++i)
    {
        argv[i] = argv[i + 1];
    }
    argv[argc - 1] = word;
    r = 1;
    while(1)
    {
        r = buf_getline(STDIN_FILENO, buf, word);
        if(r == 0)
            break;
        //r = -1;
        if (r < 0) {
            buf_free(buf);
            buf_free(buf2);
            exit(1);
        }

        word[r - 1] = 0;
        //printf("|%s|\n", word);
        int sp_res = spawn(argv[0], argv);
        if(sp_res == 0)
        {
            word[r - 1] = '\n';
            word[r] = 0;
            //printf("|%s|\n", word);
            w = buf_write(STDOUT_FILENO, buf2, word, r);
            if(w < 0)
            {
                buf_free(buf);
                buf_free(buf2);
                exit(1);
            }

        }
    }
    buf_free(buf2);
    buf_free(buf);
    return 0;
}
