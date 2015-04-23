#include <stdio.h>
#include "cd.h"
/* If our preference is using different files:

#include "exit.h"
#include "cd.h"
#include "checkEnv.h"

*/


int main(void){
    printf("lets do this! #shell #deadline20maj");
    int y = foo(5);
    printf("lol\n\n\n");
    printf("%d\n", y);
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


// TODO: Witch craft
/*
Detection of terminated background process should be implemented by two
mechanisms where one selects which should be compiled at compilation time.
The mechanisms are ordinary polling and detection by signals sent from the
child processes. The user should be able to select which mechanism to use
by defining a macro SIGDET=1 at compilation time to have termination detected
by signals. If SIGDET is undefined or equals zero then termination should be
detected by polling.
*/