#include "../lib/bufio.h"
#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

const int BUF_SIZE = 4096;
const int MAX_PROGRAMS_CNT = 100;
struct execargs_t* programs[MAX_PROGRAMS_CNT];
int programs_cnt = 0;

void parse_line(char* line, int length) {
    int argc = 0;
    int word_start = 0;
    int i = 0;
    while(i < length - 1 && (line[i + 1] == ' ' || line[i + 1] == 0))
    {
        i++;
        word_start = i + 1;
    }
    for(; i < length; ++i) {
        if((line[i] == ' ' || i == length - 1) && i - word_start > 0) {
            if(argc == 0) {
                programs[programs_cnt] = execargs_new();
                programs[programs_cnt]->file = malloc(i - word_start + 1);
                programs[programs_cnt]->args[0] = malloc(i - word_start + 1);
                memcpy(programs[programs_cnt]->args[0], line + word_start, i - word_start);
                memmove(programs[programs_cnt]->file, line + word_start, i - word_start);
                programs[programs_cnt]->file[i - word_start] = 0;
                programs[programs_cnt]->args[0][i - word_start] = 0;
                //printf("pr: (%s)\n", programs[programs_cnt]->file);
                programs_cnt++;
                while(i < length - 1 &&  (line[i + 1] == ' ' || line[i + 1] == 0))
                    i++;
                word_start = i + 1;
                argc = 1;

            } else {
                programs[programs_cnt - 1]->args[argc] = malloc(i - word_start + 1);
                memmove(programs[programs_cnt - 1]->args[argc], line + word_start, i - word_start);
                programs[programs_cnt - 1]->args[argc][i - word_start] = 0;
                //printf("arg: %s\n", programs[programs_cnt - 1]->args[argc]);
                while(i < length &&  (line[i + 1] == ' ' || line[i + 1] == 0))
                    i++;
                word_start = i + 1;
                argc++;
                programs[programs_cnt - 1]->argc = argc;

            }
        } else if(line[i] == '|') {
            argc = 0;
            while(i < length - 1 && (line[i + 1] == ' ' || line[i + 1] == 0))
                i++;
            word_start = i + 1;
        }
    }
}

static void sig_handler(int signo) {
    if (signo == SIGINT) {
        printf("1\n");
        kill(0,SIGINT);
    } else if (signo == SIGQUIT) {
        exit(0);
    }
}

int main() {
    struct buf_t *buf = buf_new(BUF_SIZE);
    if(buf == NULL)
        return -1;
    char line[BUF_SIZE + 1];




    int r = 1,w;
    while(r > 0) {
        w = write(STDOUT_FILENO, "$", 1);
        if(w < 0)
            return -1;
        programs_cnt = 0;
        r = buf_getline(STDIN_FILENO,buf,line);
        line[r - 1] = 0;
        parse_line(line, r);
        if(programs_cnt > 0) {
            /*for(int i = 0; i < programs_cnt; ++i) {
                printf("pr: (%s)",programs[i]->file);
                for(int j = 0; j < programs[i]->argc; ++j)
                    printf(" (%s)", programs[i]->args[j]);
                printf("\n");
            }*/
            runpiped(programs, programs_cnt);
            for (int i = 0; i < programs_cnt; ++i) {
                free(programs[i]);
            }
            printf("\nreturned from pipe");
        }
    }
    buf_free(buf);
    return 0;
    
}
