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
     struct command *head;
} command;

typedef struct builtins 
{
     char *name;	/* name of function */
     int (*f)(int argc, char **argv);	/* function to execute for the built-in command */
} builtins;



command *parse(char *input);
void freeCmd(command *cmd);
void printCmd(command *cmd);
void processPipe(command *cmd, char **argv);
void process(command *cmd, char **argv);
int getNumArgs(command *cmd);
int cd_cmd(int argc, char **argv);
int exit_cmd(int argc, char **argv);
void buildFunctionTable(void);
int isBuiltIn(command *cmdName);
int isEmpty(char *input);
