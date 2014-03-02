// Group Members : Alexander DeOliveira, Elana Braff, Jesse Huang
#include "shell.h"
#define NUM_COMMANDS 2

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
     memset(cmd->argv, 0, 50);
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
                    (cmd->argc)++;
                    //Create new command struct
                    input[count] = '\0';
                    cmd->next = (command *)malloc(sizeof(struct command));
                    cmd = cmd->next;
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
     (cmd->argc)++;
     return head;
     
}

struct builtins functionTable[NUM_COMMANDS];

// counts the number of arguments for a command
int getNumArgs(command *cmd) 
{
     return cmd->argc - 1;
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

void loop_pipe(command *cmd) 
{
     int   fd[2];
     pid_t pid;
     int   fd_in = 0;
     
     while (cmd != NULL)
     {
          pipe(fd);
          pid = fork();
          if (pid == -1)
          {
               exit(EXIT_FAILURE);
          }
          else if (pid == 0)
          {
               dup2(fd_in, 0); //change the input according to the old one 
               if (cmd->next != NULL)
                    dup2(fd[1], 1);
               close(fd[0]);
               execvp(cmd->argv[0], cmd->argv);
               exit(EXIT_FAILURE);
          }
          else
          {
               wait(NULL);
               close(fd[1]);
               fd_in = fd[0]; //save the input for the next command
               cmd = cmd->next;
          }
     }
}

// process command

void process(command *cmd) 
{
     /* // check for exit */
     /* if (strcmp(cmd->argv[0], "exit") == 0) */
     /*      exit(0); */
     /* // check for cd */
     /* else if (strcmp(cmd->argv[0], "cd") == 0) */
     /* { */
     /*     cd(cmd); */
     /*     return; */
     /* } */
     
     int i;
     int executedBuiltin = 0;
     pid_t pid;
     int status;
     int fd[2]; 
     int fd_in = 0;
     
     // check for built in commands
     for (i = 0; i < NUM_COMMANDS; i++) 
     {
          if (strcmp(functionTable[i].name, cmd->argv[0]) == 0)
          {
               (*(functionTable[i].f))(cmd);
               executedBuiltin = 1;
               break;
          }
     }
     if (!executedBuiltin)
     {
          loop_pipe(cmd);

          /* pid = fork(); */
          
          /* if (pid == 0) */
          /* { */
          /*      // child process */
          /*      execvp(cmd->argv[0], cmd->argv); */
          /*      perror(cmd->argv[0]); */
          /*      exit(1); */
          /* } */
          /* pid = wait(&status); */
          /* if (pid == -1) */
          /*      exit(1); */
          /* if (WIFEXITED(status)) { */
          /*      printf("process %d exit with status %d\n", pid, WEXITSTATUS(status)); */
          /* } */
     }     
     
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
               
               cmd = parse(input);
               printCmd(cmd);
               
               if(cmd)
                    process(cmd);
               else if (isatty(0))
                    printf("$  Invalid Command!!!!");
               else	
                    printf("  Invalid Command!!!!");
               
               
               
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
