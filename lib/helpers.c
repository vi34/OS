#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "helpers.h"


ssize_t read_(int fd, void *buf, size_t nbyte) {
    int n;
    ssize_t count = 0;
    while ((n = read(fd,buf + count,nbyte - count)) > 0 && count < nbyte) {
        count += n;
    }
    if(n < 0)
        return n;
    return count;
}

ssize_t write_(int fd, const void *buf, size_t nbyte) {
    int n;
    ssize_t count = 0;
    while((n = write(fd, buf + count, nbyte - count)) >= 0 && count < nbyte) {
        count += n;
    }
    if(n < 0)
        return n;
    return count;
}

ssize_t read_until(int fd, void * buf, size_t nbyte, char delimiter) {
    int n;
    ssize_t count = 0;

    while ((n = read(fd,buf + count,nbyte - count)) > 0 && count < nbyte) {
        count += n;
        int b = 0;
        for(int i = 0; i < n && !b; ++i)
            if(((char*)buf)[i + count - n] == delimiter)
                b = 1;
        if (b)
            break;
    }
    if(n < 0)
        return n;
    return count;
}

int spawn(const char * file, char * const argv [])
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return -1;
    } else if (pid == 0) { //child
        execvp(file,argv);
        perror("exec");
        return -1;
    } else {
        int status;
        if(wait(&status) < 0)
        {
            perror("wait");
            return -1;
        }
        return status;
    }
    return 0;
}

struct execargs_t* execargs_new() {
    struct execargs_t* p;
    p = (struct execargs_t*) malloc(sizeof(struct execargs_t));
    return p;
}

int exec(struct execargs_t* args) {
    args->args[args->argc] = 0;
    execvp(args->file, args->args);
    perror("exec");
    return -1;
}

pid_t* child_pids;
int child_num;

static void sig_handler(int signo) {
    if(signo == SIGINT) {
        for(int i = 0; i < child_num; ++i) {
            kill(child_pids[i], SIGKILL);
        }
    } else if (signo == SIGQUIT) {
        exit(0);
    }
}



static void sig_handler2(int signo) {
    if(signo == SIGINT) {
        //write(STDERR_FILENO, "3\n", 2);
        exit(0);
    } else if (signo == SIGQUIT) {
        exit(0);
    }
}


int runpiped(struct execargs_t** programs, size_t n) {
    int pipes[n - 1][2];
    pid_t pids[n];

    child_num = n;
    child_pids = pids;

    if (signal(SIGINT, sig_handler) == SIG_ERR || signal(SIGQUIT, sig_handler) == SIG_ERR)
        return -1;

    int r;
    r = pipe(pipes[0]);
    if (r < 0)
        return -1;
    pids[0] = fork();
    if (pids[0] < 0)
        return -1;
    if (pids[0] == 0) { // child
        if (signal(SIGINT, sig_handler2) == SIG_ERR)
            return -1;
        if(n > 1 && dup2(pipes[0][1], STDOUT_FILENO) < 0)
            return -1;

        close(pipes[0][1]);
        close(pipes[0][0]);

        exec(programs[0]);
    } else {
        for(int i = 1; i < n - 1; ++i)
        {
            r = pipe(pipes[i]);
            pids[i] = fork();
            if(pids[i] == 0) { //child
                if (signal(SIGINT, sig_handler2) == SIG_ERR)
                    return -1;
                if(dup2(pipes[i - 1][0], STDIN_FILENO < 0))
                    return -1;
                if(dup2(pipes[i][1], STDOUT_FILENO) < 0)
                    return -1;

                for(int j = 0; j < n - 1; ++j) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                exec(programs[i]);
            }
        }

        if (n > 1) {
            pids[n - 1] = fork();
            if (pids[n - 1] == 0) {
                if (signal(SIGINT, sig_handler2) == SIG_ERR)
                    return -1;
                if(dup2(pipes[n - 2][0], STDIN_FILENO < 0))
                    return -1;

                for(int i = 0; i < n - 1; ++i) {
                    close(pipes[i][0]);
                    close(pipes[i][1]);
                }
                exec(programs[n - 1]);
            }
        }

        for(int i = 0; i < n - 1; ++i) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        int res;
        wait(&res);
        /*for(int i = 0; i < n; ++i) {
            kill(pids[i],SIGINT);
        }*/
        for(int i = 0; i < n - 1; ++i) {
            wait(&res);
        }

    }

    return 0;
}
