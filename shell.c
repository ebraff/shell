// Group Members : Alexander DeOliveira, Elana Braff, Jesse Huang
#include "shell.h"
#define NUM_COMMANDS 2

// takes in pointer to user input,
// and pointer to cmd (assumes it is already allocated)
command *parse(char *input) 
{
     int count = 0, arg = 0, start = 0,quote = 0;
     command *cmd;
     if (!(cmd = (command *)malloc(sizeof(struct command)))) {
          printf("bad memory allocation\n");
          exit(1);
     }
     command *head = cmd;
     memset(cmd->argv,0,51);
     cmd->argv[0] = input;
     cmd->argc = 1;
     cmd->next = NULL;
     
     
     if (input[strlen(input)-1] == '\n') {
          input[strlen(input)-1] = '\0';
     }
     
	// remove extra spaces
	while(input[count] == ' ')
		count++;

	if (input[count] == 0)
	{
		cmd->argc = 0;
		return cmd;
	}
     
     
     for(; input[count] != '\0' || count < strlen(input); count++)
     {
		  if (cmd->argc == 50)
		  {
			  printf("Too many arguments!\n");
			  head->argc = 0;
			  return;
		  }
          // what is the current character
          switch(input[count])
          {
          case ' ' :
               if (!quote) // found a space not inside of a quote
               {
                    input[count] = '\0';
                    // remove extra spaces
                    while(input[count + 1] == ' ')
                         count++;
                    
                    
                    cmd->argv[++arg] = input + count + 1;
                    if (cmd->argv[arg][0] != '|'
                        && cmd->argv[arg][0] != '\0')
                         cmd->argc++;
                    
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
                    input[count] = '\0';
                    cmd->next = (command *)malloc(sizeof(struct command));
                    cmd = cmd->next;
                    memset(cmd->argv,0,51);
                    cmd->argc = 1;
                    arg=0;
                    
                    // remove extra spaces
                    while(input[count + 1] == ' ')
                         count++;
                    
                    cmd->argv[0] = input + count + 1;
                    cmd->next = NULL;
               }
               break;
          default :
               break;
          }
     }
     if(quote)
     {
          printf("%s\n", "Invalid quotes. Exiting program");
          return NULL;
     }
     return head;
     
}

struct builtins functionTable[NUM_COMMANDS];

// counts the number of arguments for a command
int getNumArgs(command *cmd) 
{
     return cmd->argc;
}


int cd_cmd(command *cmd) 
{
     int numArgs = getNumArgs(cmd);
     if (numArgs == 1)
     {
          chdir(getenv("HOME"));
     }
     else if (numArgs == 2) 
     {
          chdir(cmd->argv[1]);                    
     }
     else                       /* barf */
     {
          fprintf(stderr, "cd: too many arguments\n");
          
          
          fprintf(stderr, "                  BBEEEUUUUUUAAAAAHHHHH...\n");
          fprintf(stderr, "\n");
          fprintf(stderr, "                    %%%%%%\n");
          fprintf(stderr, "                   %%%% = =\n");
          fprintf(stderr, "                   %%C    >\n");
          fprintf(stderr, "                    _)' _( .' ,\n");
          fprintf(stderr, "                 __/ |_/\\   \" *. o\n");
          fprintf(stderr, "                /` \\_\\ \\/     %`= '_  .\n");
          fprintf(stderr, "               /  )   \\/|      .^',*. ,\n");
          fprintf(stderr, "              /' /-   o/       - \" % '_\n");
          fprintf(stderr, "             /\\_/     <       = , ^ ~ .\n");
          fprintf(stderr, "             )_o|----'|          .`  '\n");
          fprintf(stderr, "         ___// (_  - (         \\\n");
          fprintf(stderr, "        ///-(    \\'   \\\\ b'ger\n");
     }
     
}

int exit_cmd(command *cmd) 
{
     // free everything
     freeCmd(cmd);
     exit(0);
}

void buildFunctionTable(void) 
{
     functionTable[0].name = "cd";
     functionTable[0].f = &cd_cmd;
     functionTable[1].name = "exit";
     functionTable[1].f = &exit_cmd;
}



// frees the command from memory
void freeCmd(command *cmd) 
{
     while(cmd!=NULL){
          command *temp = cmd;
          cmd=cmd->next;
          free(temp);
     }     
}

void printCmd(command *cmd)
{
     int i;
     while (cmd)
     {
          printf("--------\n");
          printf("num args: %d\n",cmd->argc);
          for (i = 0; i < cmd->argc; i++)
          {
               printf("arg %d:\t%s\n",i,cmd->argv[i]);
          }
          printf("--------\n");
          cmd=cmd->next;
     }
     
}


// process command

void process(command *cmd) 
{
     
     int i;
     int executedBuiltin = 0;
     int pid, status, firstCmd = 1, numThreads = 0;
     command *prev, *head = cmd;
     
     while (cmd){
		// check for built in commands
		 for (i = 0; i < NUM_COMMANDS; i++) 
		 {
			  if (strcmp(functionTable[i].name, cmd->argv[0]) == 0)
			  {
				   (*(functionTable[i].f))(cmd);
				   executedBuiltin = 1;
				   return;
			  }
		 }
		 
		if (!executedBuiltin)
		{
			pipe(cmd->pipe);
			
			switch (pid = fork()){
			  
			case 0: /* child */ 
				//printf("child\n");
				if(firstCmd == 1){ /* First command */
                    if(cmd->next) /* if there is a pipe */
                        dup2(cmd->pipe[1],1);
                    close(cmd->pipe[0]);
                    firstCmd = 0;
				}
				else if(cmd->next != NULL){ /* Not first or last command */
					dup2(prev->pipe[0],0);
                    close(prev->pipe[1]);
					dup2(cmd->pipe[1],1);
                    close(cmd->pipe[0]);
				}
				else if(cmd->next == NULL){ /* last command */
					dup2(prev->pipe[0],0);
                    close(prev->pipe[1]);
                    close(cmd->pipe[0]);
                    close(cmd->pipe[1]);
				}
				
				execvp(cmd->argv[0], cmd->argv);
				perror(cmd->argv[0]);
				exit(1);
			 
			default: /*parent */
				//printf("parent\n");
				break;
				
			case -1:
				perror("error forking");
				exit(1);
			}
			
		
		} 
		prev = cmd;
		cmd = cmd->next;
		executedBuiltin = 0;

     }
    
    while (head) 
    {
		close(head->pipe[0]);
		close(head->pipe[1]);
		head = head->next;
	}  
    while ((pid = wait(&status)) != -1)	/* pick up all the dead children */
		fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
}



int main(int argc, char **argv)
{
     
     char input[1024];
     // cmdlist holds all of the commands
     
     // cmd points to current command structure
     command *cmd = 0;

     buildFunctionTable();
     
     if (isatty(0))
          printf("$  ");
     
     while(fgets(input, 1024, stdin) != NULL) 
     {     
          if (strlen(input) < 2)
               printf("$  ");
          
          else
          {
           // printf("%s \n", input);
            cmd = parse(input);
            
            
            if(cmd && cmd->argc == 0)
			{
				freeCmd(cmd);
				if (isatty(0))
					printf("$  ");
				continue;
			}
			else if (cmd)
				process(cmd);
			else if (isatty(0))
                printf("$  Invalid Command!!!!");
			else	
				printf("  Invalid Command!!!!");
				
			
                
            freeCmd(cmd);
               
		   if (isatty(0))
				printf("$  ");
          }
     }     
     return 0;
}
