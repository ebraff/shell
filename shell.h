// Group Members : Alexander DeOliveira, Elana Braff, Jesse Huang
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct command
{
	char *argv[50];
	int argc;
	struct command *next;
} command;

typedef struct builtins 
{
     char *name;	/* name of function */
     int (*f)();	/* function to execute for the built-in command */
} builtin;


command *parse(char *input);
void freeCmd(command *cmd);
void printCmd(command *cmd);
void process(command *cmd);
void cd(command *cmd);
