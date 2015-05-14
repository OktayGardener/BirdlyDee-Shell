/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>

/* Definitions */
#define MAX_INPUT 80
#define PATH_WORKING_DIRECTORY 80
#define FOREVER '\0'
#define DELIMS " \t\r\n"
#define READ 0
#define WRITE 1
#define SIGDET 0


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




/*void kill_child(pid_t child_pid){
    kill(child_pid, SIGKILL);
}
*/

void changedir(char *args){
    char *cmd;
    int chdirsuccess;

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
        chdirsuccess = chdir(getenv("HOME"));
        if (errno || chdirsuccess == -1) perror("Command failed");
        errno = 0;
    } else {
        chdirsuccess = chdir(cmd);
        if (errno || chdirsuccess == -1) perror("Command failed");
        errno = 0;
    }
}


void handle_sigchld(int sig) {
  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {
    printf("\nBackground process terminated\n");
}

}

void  handle_sigint(int sig)
{
     signal(SIGINT, handle_sigint);
     signal(SIGCHLD, SIG_IGN);
     printf("\n");

}


void new_process(char inbuffer[], bool background) {
    clock_t start;
    clock_t end;
    float seconds;

    pid_t pid;
    if(SIGDET == 1){
    signal(SIGCHLD, handle_sigchld);
    signal(SIGINT, handle_sigint);
    }
    pid = fork();
    start = clock();

    if(pid >= 0){
        printf("\n----------------------------\n");
        printf("PROCESS STATUS:\n");
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
                if(SIGDET == 0){
                waitpid(pid, &returnstatus, 0);
                }
                end = clock();
                seconds = (float)(end - start) / CLOCKS_PER_SEC;
                printf("Foreground process:\n");
                printf("Child process took: %f s\n", seconds);
            } else {

            }

            if (returnstatus == 0){
                printf("Child terminated normally\n");
            }

            if (returnstatus == 1){
                printf("Child terminated with errors\n");
            }

        printf("----------------------------\n\n");

            return;
        }
    }
}

int piper(char *command[], char *grepargs[]){
    int pipe_A[2];
    int pipe_B[2];
    int pipe_C[2];
    int status;
    int pipereturnA;
    int pipereturnB;
    int pipereturnC;
    bool argument = false;
    pid_t pid_1, pid_2, pid_3, pid_4;
    errno = 0;

    printf("PIPEEEER\n");
    printf("command0: %s\n", command[0]);
    printf("command1: %s\n", command[1]);
    printf("command2: %s\n", command[2]);


  if(strcmp(command[3], "nothing") != 0){
        printf("command3: %s\n", command[3]);
        printf("ARGUMENT = TRUE\n");
        argument = true;
  }

    pipereturnA = pipe(pipe_A);
    pipereturnB = pipe(pipe_B);

    if (argument){
        printf("NÄMEN ETT FJÄRDE ARGUMENT\n");
        pipereturnC = pipe(pipe_C);
        if (pipereturnC == -1) perror("Fail");
    }
    if (pipereturnA == -1 || pipereturnB == -1) perror("Fail");

    printf("pipa0 %d pipa1 %d\n", pipe_A[READ], pipe_A[WRITE]);

    if ((pid_1 = fork()) == 0){
        printf("PID_1\n");
        dup2(pipe_A[WRITE], WRITE);
        close(pipe_A[WRITE]);
        close(pipe_A[READ]);
        printf("pid_1:::\n");
        execlp(command[0], command[0], NULL); /*printenv*/
        if(errno != 0) return -1;
    }
    /*if((pid_grep = fork()) == 0) {

    } */
    if((pid_2= fork()) == 0){
        printf("PID2\n");
        dup2(pipe_A[READ],READ);
        close(pipe_A[WRITE]);
        close(pipe_A[READ]);
        dup2(pipe_B[WRITE], WRITE);
        close(pipe_B[WRITE]);
        close(pipe_B[READ]);

      if(argument) {
        /* grep .c */
            /* execlp(grep,      grep,          -c, NULL);*/
            execvp(command[1], grepargs); /*grep c*/
      } else {
        /* sort */
            execlp(command[1], command[1], NULL); /*sort*/
      }
       if(errno != 0) return -1;
    }

  if((pid_3 = fork()) == 0){

      printf("PID3\n");
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

      if(errno != 0) return -1;

  }

    if (argument){

        if ((pid_4 = fork()) == 0){
             printf("PID4\n");

            dup2(pipe_C[READ], READ);
            close(pipe_A[WRITE]);
            close(pipe_A[READ]);
            close(pipe_B[WRITE]);
            close(pipe_B[READ]);
            close(pipe_C[WRITE]);
            close(pipe_C[READ]);
            execlp(command[3], command[3], NULL);           /*pager*/
        }

    }
    close(pipe_A[WRITE]);
    close(pipe_A[READ]);
    close(pipe_B[WRITE]);
    close(pipe_B[READ]);

    if(SIGDET == 0){
      printf("\nPolling processes...\n");
      wait(&status);
      wait(&status);
      wait(&status);
    } else {
      printf("\nDetecting processes with signals...\n");
     /* SIGDET == 1 */
    }

    if(argument){
        close(pipe_C[WRITE]);
        close(pipe_C[READ]);
        if (SIGDET == 0)
            wait(&status);
    }
    return 0;
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
    if(getenv("PAGER") == NULL) {
        pager = "less";
    } else {
        pager = getenv("PAGER");
    }
    printf("%s\n", pager);
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
    printf("No argumentslol\n");

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
    printf("Argumeeents\n");
    piper(command, c);

  }

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
    signal(SIGINT, handle_sigint);
    /* Shell info */
    printf("\n\nShell for KTH OS Course, using C.\n");
    printf("\nWelcome to BirdlyDee Shell! Remember: Dee's a bird!\n");
    printf("Created by Oktay Bahceci and Simon Orresson\n\n");

    /* Set the path */

    while(FOREVER != EOF){
        /* Get the name of the current working directory, store it in pwd */
        char *returngetcwd;
        returngetcwd = getcwd(pwd, MAX_INPUT); /* char* getcwd(char* buffer, size_t size ); */

        if (returngetcwd == NULL) perror("Error");
        /* Print shell name and working directory */
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
            continue;

        }
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
              if(inbuffer[strlen(inbuffer)-1] == '&') { /*background process*/
                  inbuffer[strlen(inbuffer)-1] = '\0';
                  new_process(inbuffer, true);
              } else {          /*foreground process*/
                new_process(inbuffer, false);
              }
          }

        }
    }
    return 0;
}
