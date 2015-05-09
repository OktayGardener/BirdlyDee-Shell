/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "shell.h"

/* Definitions */
#define MAX_INPUT 80
#define PATH_WORKING_DIRECTORY 80
#define FOREVER '\0'
#define DELIMS " \t\r\n"

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


/*
main måste alltid vara längst ner
*/
int main(){
    pid_t pid;
    char inbuffer[MAX_INPUT];
    char pwd[PATH_WORKING_DIRECTORY];
    char *command;
    char *instr;
    int stdinchar = 0;
    errno = 0;

    /* Shell info */
    printf("\n\nShell for KTH OS Course, using C.\n");
    printf("\nWelcome to BirdlyDee Shell! Remember: Dee's a bird!\n");
    printf("Created by Oktay Bahceci and Simon Orresson\n\n");

    /* Set the path */
    setenv("PATH", "/bin:/usr/bin", 1);

    while(FOREVER != EOF){
        /* Get the name of the current working directory, store it in pwd */
        getcwd(pwd, MAX_INPUT); /* char* getcwd(char* buffer, size_t size ); */
        /* Print shell name and working directory */
        printf("birdly_dee:%s$ ", pwd);
        /* Read data from stdin, if we can't, stop the program */
        if (!fgets(inbuffer, MAX_INPUT, stdin)){
            printf("\nFATAL ERROR: Could not read data from stdin.\n");
            break;
        }

        /* Last character in the inbuffer is a nullpointer */
        inbuffer[strlen(inbuffer)-1] = '\0';

        /* Retrieve number of characters of the input */
        while(inbuffer[stdinchar] == ' ' && stdinchar < strlen(inbuffer)) stdinchar++;
        /* Point at the adress where the char array of the input is */
        instr = &inbuffer[stdinchar];

        /* strcmp(x,y) == 0 if the strings match */
        if(strcmp(instr, "exit") == 0) break;

        if(instr[0] == 'c' && instr[1] == 'd'){
                changedir(instr);
            }
        }
        /* create new process*/

    }

