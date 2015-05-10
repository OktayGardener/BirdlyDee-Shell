#include "checkEnv.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void checkEnv(char *args){
    char *cmd;
    /* get argument after printenv */
    cmd = strtok(args, " | ");

    while(cmd != NULL){
        printf("%s\n", cmd);
        cmd = strtok(NULL, " | ");
    }
    printf("\nOMGPAGERRR: %s", getenv("PAGER"));
}