#ifndef SHELL_H
#define SHELL_H

int count_pipes(char *s);
int count_args(char *s);
void create_process(char *inbuff);
void changedir(char *args);
//void kill_child(int child_pid);
char** makecommands(char inbuffer[]);
//void newprocess(char inbuffer[]);
void checkEnv(char args[]);

#endif
