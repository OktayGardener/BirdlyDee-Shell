/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "shell.h"
/* If our preference is using different files:

#include "exit.h"
#include "cd.h"
#include "checkEnv.h"

*/

/* Definitions */
#define MAX_INPUT 80
#define DELIMS " \t\r\n"


void input(int argc, char *argv[]){
    char *cmd;
    char line[MAX_INPUT];
    char c = '\0';

    while(c != EOF){
        printf("dees_a_bitch_shell > ");
        /* Too big input, exit */
        if (!fgets(line, MAX_INPUT, stdin))
            break;

        if ((cmd = strtok(line, DELIMS))) {
            printf("%s\n", cmd);
          // Clear errors
          errno = 0;
          /* Change directory */
            if (strcmp(cmd, "cd") == 0) {
                /* get the arguments after cd */
                char *arg = strtok(0, DELIMS);

                if (!arg)
                    fprintf(stderr, "cd missing argument\n");
                else
                    chdir(arg);
            /* Exit the shell */
            } else if (strcmp(cmd, "exit") == 0) {
                printf("bye dee you fucking bitch");
                break;

            } else system(line);
            /* Anything else than ls, cd, exit */
            if (errno) perror("Command failed");
            }
    }
}
/*
void input2(int argc, char *argv[]){
    char *cmd;
    char line[MAX_INPUT];
    char c = '\0';

    while(c != EOF){
        printf("dees_a_bitch_shell > ");
        if (!fgets(line, MAX_INPUT, stdin))
            break;
        cmd = strtok(line, DELIMS);
        int cd = strcmp(cmd, "cd");

        switch(*cmd){
            case cd == 0:
                if (!arg)
                    fprintf(stderr, "cd missing argument\n");
                    break;
                else
                    chdir(arg);
                    break;
        }
    }
}
*/

/*
main måste alltid vara längst ner
*/
int main(int argc, char *argv[]){
    input(argc, argv);
    return 0;
}


    /* TODO: Exit
        which terminates all remaining processes started
        from the shell in an orderly manner before exiting the shell itself

        Idea:
            clean memory, kick out, hehe
    */
    /*void exit(){

    }
    */

    /* TODO: cd
        which should behave exactly as a normal built-in command "cd"
        with the exception that you do not need explicitly
        to update environment variables.

        Idea:
            recursive
    */
    /*
    void cd(){

    }
    */

    /* TODO: checkEnv
    a built-in command "checkEnv" which should execute "printenv | sort | pager"
    if no arguments are given to the command. If arguments are passed to the
    command then "printenv | grep <arguments> | sort | pager" should be executed.
    The pager executed should be selected primarily based on the value of the
    users "PAGER" environment variable. If no such variable is set then one should
    first try to execute "less" and if that fails "more".
    Any errors in the execution of the pipeline should be handled in a
    nice manner (i.e. similar to how your normal shell handles errors
    in the execution of a manually set up pipeline).
    */

    /*
    void checkEnv(){
        printf("hejhejhej, jag är bara fjorton år, snälla, printa, sorta och pagea mig #populär");
    }
    */


    /* TODO: Witch craft
    Detection of terminated background process should be implemented by two
    mechanisms where one selects which should be compiled at compilation time.
    The mechanisms are ordinary polling and detection by signals sent from the
    child processes. The user should be able to select which mechanism to use
    by defining a macro SIGDET=1 at compilation time to have termination detected
    by signals. If SIGDET is undefined or equals zero then termination should be
    detected by polling.
    */
