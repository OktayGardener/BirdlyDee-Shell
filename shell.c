/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* Definitions */
#define MAX_INPUT 80
#define PATH_WORKING_DIRECTORY 80
#define FOREVER '\0'
#define DELIMS " \t\r\n"

extern char **environ;

void checkEnv(char *args, char **envp, char **env){
    char *cmd;
    int PAGER;
    /* get argument after printenv */
    cmd = strtok(args, " | ");

    if(strcmp(cmd, "printenv") == 0) {

    }
    while(cmd != NULL){
        printf("%s\n", cmd);
        cmd = strtok(NULL, " | ");
    }
    printf("\nOMGPAGERRR: %s", getenv("PAGER"));
    /* Prints the user ENVIROMENT variable */
    if(strcmp(getenv("PAGER"), "less") ){
        PAGER = 80;
    }
    for (env = envp; *env != 0; env++)
    {
        char* thisEnv = *env;
        printf("%s\n", thisEnv);
    }
}

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
        if (errno) perror("Command failed");
    } else {
        chdir(cmd);
        if (errno) perror("Command failed");
    }
}

void newprocess (char inbuffer[]) {
		pid_t pid;
		printf("LOL");
		pid = fork();
		if(pid >= 0){
			printf("Fork successfull!\n");
			if(pid == 0) { //child process
				printf("Child process\n");

				char ** res; NULL;
				char * p = strtok(inbuffer, " ");
				int n_spaces= 0, i;

				while (p){
					res = realloc (res, sizeof(char*) * ++n_spaces);
					if (res == NULL)
						exit(-1);

					res[n_spaces-1] = p;

					p = strtok (NULL, " ");
				}
				res = realloc (res, sizeof (char*) * ++n_spaces);
				res[n_spaces] = NULL;
				printf(res[1]);

				execvp(inbuffer, res);

			} else { //parent process
				printf("Parent Process\n");
			}
		}
}


/*
main måste alltid vara längst ner
*/
int main(int argc,char** envp){
    pid_t pid;
    char inbuffer[MAX_INPUT];
    char pwd[PATH_WORKING_DIRECTORY];
    char *command;
    char *instr;
    char **env = 0;
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
        } else if(instr[0] == 'p' && instr[1] == 'r' && instr[2] == 'i' && instr[3] == 'n'
            && instr[4] == 't' && instr[5] == 'e' && instr[6] == 'n' && instr[7] == 'v') {

        checkEnv(instr, environ, env);
        } else {
			newprocess(inbuffer);
		}

    }
        /* create new process*/
}

