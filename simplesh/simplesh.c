#include "../lib/bufio.h"
#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const int BUF_SIZE = 4096;
const int MAX_PROGRAMS_CNT = 100;
struct execargs_t* programs[MAX_PROGRAMS_CNT];
int programs_cnt = 0;

void parse_line(char* line, int length) {
    int argc = -1;
    int word_start = 0;
    for(int i = 0; i < length; ++i) {
        if(line[i] == ' ' || i == length - 1) {
            if(argc == -1) {
                programs[programs_cnt] = execargs_new();
                programs[programs_cnt]->file = malloc(i - word_start + 1);
                memmove(programs[programs_cnt]->file, line + word_start, i - word_start);
                programs[programs_cnt]->file[i - word_start] = 0;
                printf("pr: %s\n", programs[programs_cnt]->file);
                programs_cnt++;
                while(i < length - 1 && line[i + 1] == ' ')
                    i++;
                word_start = i + 1;
                argc = 0;

            } else {
                programs[programs_cnt - 1]->args[argc] = malloc(i - word_start + 1);
                memmove(programs[programs_cnt - 1]->args[argc], line + word_start, i - word_start);
                programs[programs_cnt - 1]->args[argc][i - word_start] = 0;
                printf("arg: %s\n", programs[programs_cnt - 1]->args[argc]);
                while(i < length && line[i + 1] == ' ')
                    i++;
                word_start = i + 1;
                argc++;

            }
        } else if(line[i] == '|') {
            argc = -1;
            while(i < length - 1 && line[i + 1] == ' ')
                i++;
            word_start = i + 1;
        }
    }
}

int main() {
    struct buf_t *buf = buf_new(BUF_SIZE);
    if(buf == NULL)
        return -1;
    char line[BUF_SIZE + 1];


    int r,w;
    while(1) {
        w = write(STDOUT_FILENO, "$ ", 2);
        if(w < 0)
            return -1;
        r = buf_getline(STDIN_FILENO,buf,line);
        parse_line(line, r);
        //runpiped(programs, programs_cnt);

    }
       return 0;
    
}
