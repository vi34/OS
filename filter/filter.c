#include "../lib/helpers.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const size_t SIZE = 4096;

int main() {
    char* args[] = {"ls", ".f", NULL};
    int res = spawn("ls", args);
    printf("res: %d", res);

    return 0;
}
