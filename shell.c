// Group Members : Alexander DeOliveira, Elana Braff, Jesse Huang
#include "shell.h"

int main(int argc, char **argv)
{
	if (isatty(0))
		printf("$  ");
	
	char input[1024];
	fgets(input, 1024, stdin);

	// cmdlist holds all of the commands
	command *cmdlist = (command *)malloc(sizeof(struct command));
	// cmd points to current command structure
	command *cmd = cmdlist;

	cmd->argv[0] = input;
	cmd->argc = 1;

	int count, arg = 0, start = 0,quote = 0;
	for(count = 0; input[count] != 0 || count < strlen(input); count++)
	{
		// what is the current character
		switch(input[count])
		{
			case ' ' :
				if (!quote) // found a space not inside of a quote
				{
					input[count] = 0;
					// remove extra spaces
					while(input[count + 1] == ' ')
						count++;

					cmd->argv[cmd->argc++] = input + count + 1;
					//cmd->argc++;
				}
				break;				
			case '\"' : // falls into the next case
			case '\'' :
				if (quote && input[start] == input[count]) // check the quote flag to see if we are looking for a matching quote
				{
					quote = 0;
				}
				else if (!quote)// found the start of a quoted argument
				{
					quote = 1;
					start = count;
				}
				break;
			case '|' :
				if (!quote) // found a pipe not inside of a quote 
				{
					//Create new command struct
					input[count] = 0;
					cmd->next = (command *)malloc(sizeof(struct command));
					cmd = cmd->next;
					cmd->argc = 1;

					// remove extra spaces
					while(input[count + 1] == ' ')
						count++;

					cmd->argv[0] = input + count + 1;
				}
				break;
			default :
				break;
		}
	}
	//cmd->argv[argc+1]=0;
	if(quote)
	{
		printf("%s\n", "Invalid quotes. Exiting program");
		exit(1);
	}
	int i;
	cmd=cmdlist;
	while(cmd!=NULL){
		for(i = 0; i < cmd->argc; i++)
			printf("'%s'\n", cmd->argv[i]);
		command *temp = cmd;
		cmd=cmd->next;
		free(temp);
	}

		return 0;
}
