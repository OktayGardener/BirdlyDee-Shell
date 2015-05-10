#include "cd.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void changedir(char *args){
    char *cmd;
    /* get argument after cd */
    cmd = strtok(args, "cd ");

    if(cmd == NULL){
        /* EINVAL: invalid argument (22) */
        errno = 22;
        perror("Command failed");
    }
    else if(strcmp(cmd, "~") == 0) {
        chdir(getenv("HOME"));
    } else {
        chdir(cmd);
        if (errno) perror("Command failed");
    }
}
