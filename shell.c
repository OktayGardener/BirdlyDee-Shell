/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

/* Definitions */
#define MAX_INPUT 80
#define PATH_WORKING_DIRECTORY 80
#define FOREVER '\0'
#define DELIMS " \t\r\n"
#define READ 0
#define WRITE 1


int count_pipes(char *s){
    int i, pipes = 0;
    for(i = 0; i < strlen(s); i++){
        if(s[i] == '|') pipes++;
    }
    return pipes;
}


int count_args(char *s){
    int i = 1, j = 0;
    while(i < strlen(s)){
    j++;
    while((s[i] != ' ' || s[i - 1] == '\\') && i < strlen(s)) i++;
    while(s[i] == ' ' && i < strlen(s)) i++;
    }
    return j;
}

char ** makecommands(char inbuffer[], char delim[]){
    char ** res  = NULL;
    char *  p    = strtok (inbuffer, delim);
    int n_spaces = 0;


/* split string and append tokens to 'res' */

    while (p) {
        res = realloc (res, sizeof (char*) * ++n_spaces);
        if (res == NULL)
            exit (-1); /* memory allocation failed */
        res[n_spaces-1] = p;
        p = strtok (NULL, delim);
    }

/* realloc one extra element for the last NULL */

    res = realloc (res, sizeof (char*) * (n_spaces+1));
    res[n_spaces] = 0;
    return res;
}


void kill_child(pid_t child_pid){
    kill(child_pid, SIGKILL);
}


void changedir(char *args){
    char *cmd;
    errno = 0;
    /* get argument after cd */
    cmd = strtok(args, "cd ");

    if(cmd == NULL){
        printf("%s", cmd);
        /* EINVAL: invalid argument (22) */
        errno = 22;
        perror("Command failed");
        errno = 0;
    } else if(strcmp(cmd, "~") == 0) {
        chdir(getenv("HOME"));
        if (errno) perror("Command failed");
        errno = 0;
    } else {
        chdir(cmd);
        if (errno) perror("Command failed");
        errno = 0;
    }
}


void new_process(char inbuffer[], bool background) {
    pid_t pid = fork();
    if(pid >= 0){
        printf("Fork successfull! %u\n", getpid());
        if(pid == 0) {
            /* child process */

            char ** res;
            char *p;
            int n_spaces;
            printf("Child process, pid: %u\n", getpid());
            res = NULL;
            p = strtok(inbuffer, " ");
            n_spaces = 0;

            while (p){
                res = realloc (res, sizeof(char*) * ++n_spaces);
                if (res == NULL)
                    exit(-1);

                res[n_spaces-1] = p;

                p = strtok(NULL, " ");
            }
            res = realloc (res, sizeof (char*) * ++n_spaces);
            res[n_spaces] = NULL;

            execvp(inbuffer, res);
            return;
        } else {
            /* parent process */
            int returnstatus = 0;
            printf("Parent Process, pid: %u\n", getpid());
            if (pid == -1){
                /* fork failed */
                char * errormessage = "UNKNOWN"; /* felmeddelandetext */
                if( EAGAIN == errno ) errormessage = "cannot allocate page table";
                if( ENOMEM == errno ) errormessage = "cannot allocate kernel data";
                fprintf( stderr, "fork() failed because: %s\n", errormessage );
                exit( 1 );
            }

            if(!background){
                printf("lol inte background, foreground\n");
                waitpid(pid, &returnstatus, 0);
                kill_child(pid);
            }

            if (returnstatus == 0){
                printf("Child terminated normally\n");
            }

            if (returnstatus == 1){
                printf("Child terminated with errors\n");
            }

            printf("\nkilled: childpid: %i\n", pid);

            return;
        }
    }
}

void piper(char *command[]){
  	pid_t endID;
  	printf("PIPEEEER\n");
  	printf("command0: %s\n", command[0]);
  	printf("command1: %s\n", command[1]);
  	printf("command2: %s\n", command[2]);

  	int pipe_A[2];
  	int pipe_B[2];
  	int status;
  
  	pipe(pipe_A);
  	pipe(pipe_B);
  
  	pid_t pid_A, pid_B, pid_C;
  
  	printf("pipa0 %d pipa1 %d\n", pipe_A[READ], pipe_A[WRITE]);
  	
  	if ((pid_A = fork()) == 0){
      	printf("PIDA\n");
    	dup2(pipe_A[WRITE], WRITE);
    	close(pipe_A[WRITE]);
    	close(pipe_A[READ]);
      	printf("pid_A:::\n");
      	execlp(command[0], command[0], NULL);
    } 
  	if((pid_B = fork()) == 0) {
      	printf("PIDB\n");
        dup2(pipe_A[READ],READ); 
        close(pipe_A[WRITE]);
        close(pipe_A[READ]);
      	dup2(pipe_B[WRITE], WRITE);
      	close(pipe_B[WRITE]);
      	close(pipe_B[READ]);
        execlp(command[1], command[1], NULL);
    } 
    if((pid_C = fork()) == 0){
		printf("PIDC\n");
		close(pipe_A[WRITE]);
		close(pipe_A[READ]);
		dup2(pipe_B[READ], READ);
		close(pipe_B[WRITE]);
		close(pipe_B[READ]);
		
		execlp(command[2], command[2], NULL);
	}
  	
  	close(pipe_A[WRITE]);
  	close(pipe_A[READ]);
  	close(pipe_B[WRITE]);
  	close(pipe_B[READ]);
  	wait(&status);
  	wait(&status);
  	wait(&status);
    kill_child(pid_A);
    kill_child(pid_B);
    kill_child(pid_C);
  	
  	
}

/*
which should execute "printenv | sort | pager" if no arguments are given to the command.
If arguments are passed to the command then "printenv | grep <arguments> | sort | pager" should be executed.
The pager executed should be selected primarily based on the value of the users "PAGER" environment variable.
If no such variable is set then one should first try to execute "less" and if that fails "more".
Any errors in the execution of the pipeline should be handled in a nice manner
(i.e. similar to how your normal shell handles errors in the execution of a manually set up pipeline).
*/

/* checkenv         -> printenv | sort | pager*/
/* checkenv .c      -> printenv | grep .c | sort | pager*/
void checkEnv(char args[]){
/*     char *cmd; */
    char *pager;
    pager = getenv("PAGER");
    /* pager = less if it's not set */
    /*    if(strcmp(pager, "") == 0) pager = "less";*/
    /* get argument after printenv */

    /* Case without arg */
  if(strcmp(args, "\0") == 0){
    /* checkenv         -> printenv | sort | pager*/
    char *command[3];
    command[0] = "printenv";
    command[1] = "sort";
    command[2] = "less";
    printf("No argumentslol\n");
	piper(command);
    
  } else {
    /* checkenv .c      -> printenv | grep .c | sort | pager*/
    printf("Argumeeents\n");

  }

}

void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     printf("\nOUCH, did you hit Ctrl-C?\n"
            "Do you really want to quit? [y/n] ");
     c = getchar();
     if (c == 'y' || c == 'Y')
          exit(0);
     else
          signal(SIGINT, INThandler);
     getchar(); /* Get new line character*/
}


    /*
    main måste alltid vara längst ner
    */
int main(int argc,char** envp){
    char inbuffer[MAX_INPUT];
    char pwd[PATH_WORKING_DIRECTORY];
    char *instr;
    int stdinchar = 0;
    errno = 0;
	signal(SIGINT, INThandler);
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
        if(strcmp(instr, "exit") == 0) exit(0);

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
              if(inbuffer[strlen(inbuffer)-1] == '&') {
                  inbuffer[strlen(inbuffer)-1] = '\0';
                  new_process(inbuffer, true);
              } else {
                new_process(inbuffer, false);
              }
          }
          else {
            continue;
          }
        }

    }
}

