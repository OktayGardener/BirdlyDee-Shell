/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

/* Definitions */
#define MAX_INPUT 80
#define PATH_WORKING_DIRECTORY 80
#define FOREVER '\0'
#define DELIMS " \t\r\n"

pid_t pid;

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

char ** makecommands(char inbuffer[]){
    int i;
    char *p;
    char *array[10];
    i = 0;
    p = strtok (inbuffer," | ");
    while (p != NULL)
    {
        array[i++] = p;
        p = strtok (NULL, " | ");
    }
    return array;
}

void create_process(char inbuffer[]) {
    int pfds[2];
    printf("\ninbuffer: %s\n", inbuffer);
    char **commands = makecommands(inbuffer);

    printf("\ncommand0: %s\n", commands[0]);
    printf("\ncommand1: %s\n", commands[1]);

    char *s = commands[0];
    printf("such command::::::::: %s", s);

    // pipe(pfds);

    // if (!fork()) {
    //     close(1);       /* close normal stdout */
    //     dup(pfds[1]);   /* make stdout same as pfds[1] */
    //     close(pfds[0]); /* we don't need this */
    //     execlp(commands[0], commands[0], NULL);
    // } else {
    //     close(0);       /* close normal stdin */
    //     dup(pfds[0]);   /* make stdin same as pfds[0] */
    //     close(pfds[1]); /* we don't need this */
    //     execlp("sort", "sort", NULL);
    // }

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


void kill_child(pid_t child_pid){
    kill(child_pid, SIGKILL);
}




/*void newprocess(char inbuffer[]) {

    pid = fork();
    if(pid >= 0){
        printf("Fork successfull! %u\n");
        if(pid == 0) {
            /* child process
            printf("Child process, pid: %u\n", getpid());





            execvp(inbuffer, res);
            return;

        } else {
            /* parent process
            printf("Parent Process, pid: %u\n", getpid());

            if (pid == -1){
                /* fork failed
                char * errormessage = "UNKNOWN"; /* felmeddelandetext
                if( EAGAIN == errno ) errormessage = "cannot allocate page table";
                if( ENOMEM == errno ) errormessage = "cannot allocate kernel data";
                fprintf( stderr, "fork() failed because: %s\n", errormessage );
                exit( 1 );
            }

            wait(1000);
            printf("\nkilled: childpid: %i\n", pid);
            kill_child(pid);
            return;
        }
    }
}*/

void checkEnv(char args[]){
    char *cmd;
    int PAGER;
    /* get argument after printenv */
    cmd = strtok(args, " | ");

    if(strcmp(cmd, "printenv") == 0) {
        create_process(args);
    }

    while(cmd != NULL){
        printf("%s\n", cmd);
        cmd = strtok(NULL, " | ");
    }
}

/*
main måste alltid vara längst ner
*/
int main(int argc,char** envp){
    char inbuffer[MAX_INPUT];
    char pwd[PATH_WORKING_DIRECTORY];
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
        if(strcmp(instr, "exit") == 0) exit(0);

        if(instr[0] == 'c' && instr[1] == 'd'){
            changedir(instr);
        } else if(instr[0] == 'p' && instr[1] == 'r' && instr[2] == 'i' && instr[3] == 'n'
            && instr[4] == 't' && instr[5] == 'e' && instr[6] == 'n' && instr[7] == 'v') {

            checkEnv(inbuffer);
        } else {
        /* create new process*/
          create_process(inbuffer);
      }

  }
}


