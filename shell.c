/* Includes */
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdbool.h>

/* Definitions */
#define MAX_INPUT 81
#define PATH_WORKING_DIRECTORY 80
#define FOREVER '\0'
#define DELIMS " \t\r\n"
#define READ 0
#define WRITE 1
#define SIGDET 1


/* Function for counting the number of pipes */
int count_pipes(char *s){
    int i, pipes = 0;
    /* If the '|' char is found, increment the pipes int */
    for(i = 0; i < strlen(s); i++){
        if(s[i] == '|') pipes++;
    }
    return pipes;
}


/*  Split the input into list in list with seperated commands */
char ** makecommands(char inbuffer[], char delim[]){
    char ** res  = NULL;
    char *  p    = strtok (inbuffer, delim);
    int n_spaces = 0;


    /* Split string and append tokens to 'res' */
    while (p) {
        res = realloc (res, sizeof (char*) * ++n_spaces);
        if (res == NULL)
            exit (-1); /* memory allocation failed */
        res[n_spaces-1] = p;
        p = strtok (NULL, delim);
    }

    /* Realloc one extra element for the last NULL */
    res = realloc (res, sizeof (char*) * (n_spaces+1));
    res[n_spaces] = 0;
    return res;
}

/* ERROR HANDLING */
/* Exit-function, bound to signal for all processes */
void exitshell(){
    /* Get the parent process id and kill it,
    killing the children in the process */
    pid_t pid;
    int status;
    signal(SIGQUIT, SIG_IGN);
    kill(-getpid(), SIGQUIT);
    pid = -1;
    printf("Thank you, ");
    sleep(1); /* Wait for everything to shutdown. */
    printf("come again!\n");
    while(1){
		pid = waitpid(-1, &status, WNOHANG);
		if(pid > 1) {
			if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    printf("\nProcess %d terminated.\n", pid);
			}
        } else {
			break;
        }
	}

    exit(0);
}


/* Function for changing the current working directory */
void changedir(char *args){
    /* Local variables */
    char *cmd;
    int chdirsuccess;
    /* Reset errno */
    errno = 0;
    /* Get argument after cd */
    cmd = strtok(args, "cd ");
    /* If cd has no arguments */
    if(cmd == NULL){
        printf("%s", cmd);
        /* EINVAL: invalid argument (22) */
        errno = 22;
        /* Throw the error for the user */
        perror("Command failed");
        /* Reset errno */
        errno = 0;
        /* cd ~, cd home */
    } else if(strcmp(cmd, "~") == 0) {
        /* Get the home folder from env, cd */
        chdirsuccess = chdir(getenv("HOME"));
        if (errno || chdirsuccess == -1) perror("Command failed");
        /* Reset errno */
        errno = 0;
        /* cd with argument, "cd .." */
    } else {
        chdirsuccess = chdir(cmd);
        /* Throw error if the argument was invalid */
        if (errno || chdirsuccess == -1) perror("Command failed");
        /* Reset errno */
        errno = 0;
    }
}




/*Handles all signals*/

void handler(int signum, siginfo_t* info, void* vp) {
	/* If ctrl+z is pressed, print nextline */
    if(signum == SIGTSTP) {
        printf("\n");
        return;
    }
    /* If ctrl+c is pressed, print nextline */
    if(signum == SIGINT) {
        printf("\n");
        return;
    }
    /* If child process is exited, wait for it */
    if(signum == SIGCHLD) {
        pid_t wpid;
        int status;
        wpid = waitpid(info->si_pid, &status, WCONTINUED|WNOHANG);

        /* If child process exited */

        if(wpid > 0) {

			/* If it exited normally */
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                printf("\nProcess %d terminated.\n", wpid);

                /* If it was continued, wait for it */
            } else if (WIFCONTINUED(status)) {
                wpid = waitpid(wpid, &status, WUNTRACED);
                /* If it exited normally */
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    printf("\nProcess %d terminated.\n", wpid);
                } else if (WIFSTOPPED(status)) {
                    printf("\nProcess %d stopped.\n", wpid);
                }
                /* If stopped by signal */
            } else if (WIFSTOPPED(status)) {
                printf("\nProcess %d stopped.\n", wpid);
            }
        }
        return;
    }
    return;
}

/* Register a signal handler */

void register_sighandler(int signal_code, void (*handler)(int, siginfo_t*, void*)) {
   struct sigaction signal_parameters;

   signal_parameters.sa_sigaction = handler;
   sigemptyset(&signal_parameters.sa_mask);
   signal_parameters.sa_flags = SA_SIGINFO;
   /* Error */
   if(sigaction(signal_code, &signal_parameters, (void *)0) == -1) {
       perror("sigaction error");
   }
}

void errorhandler(){
	char * errormessage = "UNKNOWN";
	if( EAGAIN == errno ) errormessage = "cannot allocate page table";
	if( ENOMEM == errno ) errormessage = "cannot allocate kernel data";
	fprintf( stderr, "fork() failed because: %s\n", errormessage );
	/* Force quit the shell */
	exitshell();
}

/* Create a new process */
void new_process(char inbuffer[], bool background) {
    /* Local system variables */

    pid_t pid;

    /* Fork a new process */
    pid = fork();
    /* Start taking the time */


    if(pid >= 0){
        /* Process information */
        /*printf("\n----------------------------\n");
        printf("PROCESS STATUS:\n");
        printf("Fork successfull! %u\n", getpid());*/
        /* Child process */
        if(pid == 0) {
			char ** res;
			if(background){
				setpgid(0, 0);
			}

            res = makecommands(inbuffer, " ");
            execvp(inbuffer, res);
            perror("exec failed");
            exit(1);
            /* Parent process */
        } else {

            int returnstatus = 0;
            /*printf("Parent Process, pid: %u\n", getpid());*/
            if (pid == -1){
                /* Fork failed, error messages */
                errorhandler();
            }

            /* Foreground process */
            if(!background){
				waitpid(pid, &returnstatus, WUNTRACED);
            }


            /*if (returnstatus == 0){
                printf("Child terminated normally\n");
            }
            if (returnstatus == 1){
                printf("Child terminated with errors\n");
            }*/

            /*printf("----------------------------\n\n");*/
        }
    }
}

/* Pipe controller function */
int piper(char *command[], char *grepargs[]){
    /* Creations of fd and pipes */
    int pipe_A[2];
    int pipe_B[2];
    int pipe_C[2];
    int status;
    int pipereturnA;
    int pipereturnB;
    int pipereturnC;
    bool argument = false;
    /* Processes */
    pid_t pid_1, pid_2, pid_3, pid_4, wpid;
    errno = 0;

    /* Print info */

    /* If grep has arguments, set the bool accordingly */
    if(strcmp(command[3], "nothing") != 0){
        argument = true;
    }

    /* Create two pipes */
    pipereturnA = pipe(pipe_A);
    pipereturnB = pipe(pipe_B);

    /* If grep <arguments> */
    if (argument){
        pipereturnC = pipe(pipe_C);
        if (pipereturnC == -1) perror("Fail when creating pipes");
    }
    /* Pipe error handling */
    if (pipereturnA == -1 || pipereturnB == -1) perror("Fail when creating pipes");



    /* Forking first process */
    if ((pid_1 = fork()) == 0){
        /* Copy the write fd from pipe_A */
        dup2(pipe_A[WRITE], WRITE);
        /* Close the read/write fd for pipe_A */
        close(pipe_A[WRITE]);
        close(pipe_A[READ]);
        /* Execute first command */
        execlp(command[0], command[0], NULL); /*printenv*/
        perror("exec1 failed"); /* only if exec fails */

    } else if (pid_1 < 0){
		perror("fork1");
		errorhandler();
	}

    if((pid_2= fork()) == 0){
        dup2(pipe_A[READ],READ);
        close(pipe_A[WRITE]);
        close(pipe_A[READ]);
        dup2(pipe_B[WRITE], WRITE);
        close(pipe_B[WRITE]);
        close(pipe_B[READ]);

        /* If grep <argument> */
        if(argument) {
            /* Example: grep .c */
            /* execlp(grep,      [grep,          -c, NULL]);*/
            execvp(command[1], grepargs); /*grep c*/
        } else {
            execlp(command[1], command[1], NULL);
        }
        perror("exec2 failed"); /* only if exec fails */
    } else if (pid_2 < 0){
		perror("fork2");
		errorhandler();
	}

    /* Third process */
    if((pid_3 = fork()) == 0){

        dup2(pipe_B[READ], READ);
        close(pipe_A[WRITE]);
        close(pipe_A[READ]);

        if (argument){
            dup2(pipe_C[WRITE], WRITE);
            close(pipe_B[WRITE]);
            close(pipe_B[READ]);
            close(pipe_C[WRITE]);
            close(pipe_C[READ]);
        } else {

            close(pipe_B[WRITE]);
            close(pipe_B[READ]);
        }

        execlp(command[2], command[2], NULL); /*pager */
        perror("exec3 failed"); /* only if exec fails */

    } else if (pid_3 < 0){
		perror("fork3");
		errorhandler();
	}
    /* If we perform grep, there will be a fourth process */
    if (argument){

        if ((pid_4 = fork()) == 0){

            dup2(pipe_C[READ], READ);
            close(pipe_A[WRITE]);
            close(pipe_A[READ]);
            close(pipe_B[WRITE]);
            close(pipe_B[READ]);
            close(pipe_C[WRITE]);
            close(pipe_C[READ]);
            execlp(command[3], command[3], NULL);           /*pager*/
            perror("exec4 failed"); /* only if exec fails */
        }
    }  else if (pid_3 < 0){
		perror("fork4");
		errorhandler();
	}
    close(pipe_A[WRITE]);
    close(pipe_A[READ]);
    close(pipe_B[WRITE]);
    close(pipe_B[READ]);




    if(argument){
        close(pipe_C[WRITE]);
        close(pipe_C[READ]);


    }
    /* Wait for all processes to finish */
	while ((wpid = wait(&status)) > 0);
	return 0;
}




/* checkenv         -> printenv | sort | pager*/
/* checkenv .c      -> printenv | grep .c | sort | pager*/
/* Handles the "printenv | sort | pager" or
"printenv | grep <arguments> | sort | pager" command */
void checkEnv(char args[]){
    /*  char *cmd; */
    char *pager;
    if(getenv("PAGER") == NULL) {
        pager = "less";
    } else {
        pager = getenv("PAGER");
    }
    /* pager = less if it's not set */
    /* Case without arg */
    if(strcmp(args, "\0") == 0){
        /* checkenv         -> printenv | sort | pager*/
        char *command[4];
        char *c[1];
        command[0] = "printenv";
        command[1] = "sort";
        command[2] = pager;
        command[3] = "nothing";

        c[0] = args;
        if(piper(command, c) == -1) {
            command[2] = "more";
            piper(command, c);
        }
    } else {
        /* checkenv .c      -> printenv | grep .c | sort | pager*/
        char *command[4];

        char **c = makecommands(args, " ");
        command[0] = "printenv";
        command[1] = "grep";
        command[2] = "sort";
        command[3] = "less";
        piper(command, c);

    }

}

/* POLLING */
void polling(){

	pid_t wpid;
	int status;
	/* Wait for all children */
	wpid = waitpid((pid_t)-1, &status, WNOHANG|WCONTINUED);
	printf("Polling processes..\n");
	/* If childprocess is finished */
    if(wpid > 0) {

		/* If childprocess was terminated normally, or by signal*/
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            printf("Process %d terminated.\n", wpid);
            /* If childprocess was stopped */
        } else if (WIFSTOPPED(status)) {
            printf("Process %d stopped.\n", wpid);
            /* If it was continued */
        } else if (WIFCONTINUED(status)) {
            while(1) {
				/* Wait for any child processes that were stopped */
                wpid = waitpid(wpid, &status, WUNTRACED);
                if(wpid > 0) {
                    if (WIFEXITED(status) || WIFSIGNALED(status)) {
                        printf("Process %d terminated.\n", wpid);
                        break;
                    } else if (WIFSTOPPED(status)) {
                        printf("Process %d stopped.\n", wpid);
                        break;
                    }
                }
            }
        }
    }


}

int main(int argc,char** envp){
    char inbuffer[MAX_INPUT];
    /* Set the path */
    char pwd[PATH_WORKING_DIRECTORY];
    char *instr;
    int stdinchar = 0;
    errno = 0;
    /* Shell info */
    printf("\n\nShell for KTH OS Course, using C.\n");
    printf("\nWelcome to BirdlyDee Shell! Remember: Dee's a bird!\n");
    printf("Created by Oktay Bahceci and Simon Orresson\n\n");

    register_sighandler(SIGINT, handler);
    register_sighandler(SIGTSTP, handler);

    #if SIGDET == 1
        register_sighandler(SIGCHLD, handler);
    #endif

    while(FOREVER != EOF){
        /* Get the name of the current working directory, store it in pwd */
        char *returngetcwd;
        returngetcwd = getcwd(pwd, MAX_INPUT); /* char* getcwd(char* buffer, size_t size ); */

        if (returngetcwd == NULL) perror("Error");
        /* Print shell name and working directory */
        sleep(1);
        printf("birdly_dee:%s$ ", pwd);
        /* Read data from stdin, if we can't, stop the program */
        if (!fgets(inbuffer, MAX_INPUT, stdin)){
            /*printf("\nFATAL ERROR: Could not read data from stdin.%d\n", errno);*/
            continue;
        }

        /* Last character in the inbuffer is a nullpointer */
        inbuffer[strlen(inbuffer)-1] = '\0';

        /* Retrieve number of characters of the input */
        while(inbuffer[stdinchar] == ' ' && stdinchar < strlen(inbuffer)) stdinchar++;
        /* Point at the adress where the char array of the input is */
        instr = &inbuffer[stdinchar];

        /* carriage return */
        if ((char) inbuffer[0] == 0) {
			if(SIGDET == 0){
				polling();
			}
            continue;

        }
        /* strcmp(x,y) == 0 if the strings match */
        if(strcmp(instr, "exit") == 0) {
            exitshell();
        }



        if(instr[0] == 'c' && instr[1] == 'd'){
            changedir(instr);
        } else if(instr[0] == 'c' && instr[1] == 'h' && instr[2] == 'e' && instr[3] == 'c'
        && instr[4] == 'k' && instr[5] == 'e' && instr[6] == 'n' && instr[7] == 'v') {

            if(strcmp(instr, "checkenv") == 0) {
                checkEnv("\0");
            } else {
                checkEnv(instr);
            }
        } else {
            /* create new process*/
            if(count_pipes(inbuffer) == 0){
                if(inbuffer[strlen(inbuffer)-1] == '&') { /*background process*/
                    inbuffer[strlen(inbuffer)-1] = '\0';
                    new_process(inbuffer, true);
                    sleep(1);
                } else {          /*foreground process*/

					/* time measuring */
					struct timeval  tv1, tv2;
					gettimeofday(&tv1, NULL);

                    new_process(inbuffer, false);
                    gettimeofday(&tv2, NULL);
                    printf ("Total time = %f seconds\n",
					(double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
					(double) (tv2.tv_sec - tv1.tv_sec));
                }
            }

        }
        if (SIGDET == 0){
			polling();
		}
    }

    return 0;
}
