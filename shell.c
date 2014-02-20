// Group Members : Alexander DeOliveira, Elana Braff, Jesse Huang
#include "shell.h"

// takes in pointer to user input,
// and pointer to cmd (assumes it is already allocated)
command *parse(char *input) 
{
     int count, arg = 0, start = 0,quote = 0;
     command *cmd;
     if (!(cmd = (command *)malloc(sizeof(struct command)))) {
          printf("bad memory allocation\n");
          exit(1);
     }
     command *head = cmd;
     cmd->argv[0] = input;
     cmd->argc = 1;
     cmd->next = NULL;
     
     
     if (input[strlen(input)-1] == '\n') {
          input[strlen(input)-1] = '\0';
     }



     for(count = 0; input[count] != '\0' || count < strlen(input); count++)
     {
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
                    
                    cmd->argv[cmd->argc + 1] = input + count + 1;
                    if (cmd->argv[cmd->argc + 1][0] != '|'
                        && cmd->argv[cmd->argc + 1][0] != '\0')
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
                    cmd->argc = 1;
                    
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
          //exit(1);
     }
     return head;
     
}

functionTable *buildFunctionTable(void) 
{
     functionTable *table = (functionTable *)malloc(sizeof(struct functionTable));

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
     if (cmd != NULL)
     {
          printf("--------\n");
          printf("num args: %d\n",cmd->argc);
          for (i = 0; i < cmd->argc; i++)
          {
               printf("arg %d:\t%s{}\n",i,cmd->argv[i]);
          }
          printf("--------\n");
     }
}


// process command

void process(command *cmd) 
{
     // check for exit
     if (strcmp(cmd->argv[0], "exit") == 0)
          exit(0);
     // check for cd
     else if (strcmp(cmd->argv[0], "cd") == 0)
     {
         cd(cmd);
         return;
     }

     int pid = fork();
     int status;
     
     if (pid == 0) 
     {
          // child process
          execvp(cmd->argv[0], cmd->argv);
          perror(cmd->argv[0]);
          exit(1);
     } 
     pid = wait(&status);
     if (pid == -1)
          exit(1);
     
     
}




void cd(command *cmd)
{
     printf("input cd code here\n");
}

int main(int argc, char **argv)
{
     
     char input[1024];
     // cmdlist holds all of the commands
     
     // cmd points to current command structure
     command *cmd = 0;
     
     if (isatty(0))
          printf("$  ");
     
     while(fgets(input, 1024, stdin) != NULL) 
     {
          
          if (strlen(input) < 2)
               printf("$  ");
          else
          {
               
               cmd = parse(input);
               
               process(cmd);
               printCmd(cmd); 
               freeCmd(cmd);
               
               /* int i,j=1; */
               
               /* while(cmd!=NULL){ */
               /*      printf("command %d: ", j++); */
               /*      for(i = 0; i < cmd->argc; i++) */
               /*           printf("'%s', ", cmd->argv[i]); */
               /*      printf("\n"); */
               /*      command *temp = cmd; */
               /*      cmd=cmd->next; */
               /*      free(temp); */
               /* } */
               
               if (isatty(0))
                    printf("$  ");
          }
     }     
     return 0;
}
