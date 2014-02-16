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
