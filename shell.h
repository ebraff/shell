/* Group Members : Alexander DeOliveira, Elana Braff, Jesse Huang */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct command
{
     char *argv[51];
     int argc;
     int pipe[2];
     struct command *next;
} command;

typedef struct builtins 
{
     char *name;	/* name of function */
     int (*f)(command *c);	/* function to execute for the built-in command */
} builtins;



command *parse(char *input);
void freeCmd(command *cmd);
void printCmd(command *cmd);
void process(command *cmd);
void cd(command *cmd);
int getNumArgs(command *cmd);
int cd_cmd(command *cmd);
int exit_cmd(command *cmd);
void buildFunctionTable(void);


