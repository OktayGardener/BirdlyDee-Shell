#include "cd.h"

void changedir(char *args){
    char *cmd;
    printf("%s\n", args);
    /* get argument after cd */
    cmd = strtok(args, "cd ");
    if(strcmp(cmd, "~") == 0) {
        chdir(getenv("HOME"));
    } else {
        chdir(cmd);
        if (errno) perror("Command failed");
    }
}