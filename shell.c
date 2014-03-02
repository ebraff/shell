/* Group Members : Alexander DeOliveira, Elana Braff, Jesse Huang */
#include "shell.h"
#define NUM_COMMANDS 2

/* takes in pointer to user input,
   and pointer to cmd (assumes it is already allocated) */

command *parse(char *input) 
{
     int count, arg = 0, start = 0,quote = 0;
     command *cmd, *head;
     if (!(cmd = (command *)malloc(sizeof(struct command)))) {
          printf("bad memory allocation\n");
          exit(1);
     }
     head = cmd;
     memset(cmd->argv, 0, 51);
     cmd->argv[0] = input;
     cmd->argc = 1;
     cmd->next = NULL;
     
     
     if (input[strlen(input)-1] == '\n') {
          input[strlen(input)-1] = '\0';
     }
     
     
     
     for(count = 0; input[count] != '\0' || count < strlen(input); count++)
     {
          if (cmd->argc == 50)
          {
               printf("Too many arguments!\n");
               head->argc = 0;
               return head;
          }
          /* what is the current character */
          switch(input[count])
          {
          case ' ' :
               if (!quote)  /* found a space not inside of a quote */
               {
                    input[count] = '\0';

                    /* remove extra spaces */
                    while(input[count + 1] == ' ')
                         count++;
                    
                    
                    
                    if (input[count + 1] != '|'
                        && input[count + 1] != '\0')
                    {
                         cmd->argc++;
                         cmd->argv[++arg] = input + count + 1;
                    }
                    
               }
               break;                          
          case '\"' : /* falls into the next case */
          case '\'' :
               if (quote && input[start] == input[count]) /* check the quote flag to see if we are looking for a matching quote */
               {
                    quote = 0;
               }
               else if (!quote) /* found the start of a quoted argument */
               {
                    quote = 1;
                    start = count;
               }
          break;
          case '|' :
               if (!quote) /* found a pipe not inside of a quote */
               {
                    (cmd->argc)++;
                    /* Create new command struct */
                    input[count] = '\0';
                    cmd->next = (command *)malloc(sizeof(struct command));
                    cmd = cmd->next;
                    cmd->argc = 1;
                    arg=0;
                    memset(cmd->argv, 0, 51);
                    
                    /* remove extra spaces */
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
          fprintf(stderr, "%s\n", "error : mismatched quotes");
          head->argc = 0;
          return head;
     }
     (cmd->argc)++;
     return head;
     
}

struct builtins functionTable[NUM_COMMANDS];

/* counts the number of arguments for a command */
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
          fprintf(stderr, "                    %%%%%%%%%%%%\n");
          fprintf(stderr, "                   %%%%%%%% = =\n");
          fprintf(stderr, "                   %%%%C    >\n");
          fprintf(stderr, "                    _)' _( .' ,\n");
          fprintf(stderr, "                 __/ |_/\\   \" *. o\n");
          fprintf(stderr, "                /` \\_\\ \\/     %`= '_  .\n");
          fprintf(stderr, "               /  )   \\/|      .^',*. ,\n");
          fprintf(stderr, "              /' /-   o/       - \" % '_\n");
          fprintf(stderr, "             /\\_/     <       = , ^ ~ .\n");
          fprintf(stderr, "             )_o|----'|          .`  '\n");
          fprintf(stderr, "         ___// (_  - (         \\\n");
          fprintf(stderr, "        ///-(    \\'   \\\\ b'ger\n");
          return 1;
     }
     return 0;
}

int exit_cmd(command *cmd) 
{
     int numArgs = getNumArgs(cmd);
     int exitCode = 0;
     if (numArgs == 2)
     {
          exitCode = atoi(cmd->argv[1]);
     }
     else if (numArgs > 2)
     {
          fprintf(stderr, "exit: too many arguments\n");
          return 1;
     }

     /* free everything */
     freeCmd(cmd);
     exit(exitCode);
}

void buildFunctionTable(void) 
{
     functionTable[0].name = "cd";
     functionTable[0].f = &cd_cmd;
     functionTable[1].name = "exit";
     functionTable[1].f = &exit_cmd;
}



/* frees the command from memory */
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

/* checks the given command to see if it is builtin if it is it will execute and return 1
 * if not it will return 0 */
int isBuiltIn(command *cmd)
{
     int i;
     for (i = 0; i < NUM_COMMANDS; i++) 
     {
          if (strcmp(functionTable[i].name, cmd->argv[0]) == 0)
          {
               (*(functionTable[i].f))(cmd);
               return 1;
          }
     }
     return 0;
}

/* helper function for process it will handle all piping */
void processPipe(command *cmd) 
{
     int fd[2];
     int pid;
     int fd_in = 0;
     int status;
     
     while (cmd != NULL)
     {
          if (isBuiltIn(cmd))
               return;

          pipe(fd);
          pid = fork();
          if (pid == -1)
          {
               exit(EXIT_FAILURE);
          }
          else if (pid == 0)
          {
               /* child process */
               dup2(fd_in, 0); 
               if (cmd->next != NULL)
                    dup2(fd[1], 1);
               close(fd[0]);
               execvp(cmd->argv[0], cmd->argv);
               exit(EXIT_FAILURE);
          }
          else
          {
               /* parent */
               while ((pid = wait(&status)) != -1)
                    fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
               
               close(fd[1]);
               fd_in = fd[0]; 
          }
          cmd = cmd->next;
     }
}


/* process command */
void process(command *cmd) 
{
     /* check for built in commands*/
     if (!isBuiltIn(cmd))
     {
          processPipe(cmd);
     }     
     
}



int main(int argc, char **argv)
{
     char input[1024];
     
     /* cmd points to current command structure */
     command *cmd = 0;
     
     buildFunctionTable();
     
     if (isatty(0))
          printf("$  ");

     /* grab input */ 
     while(fgets(input, 1024, stdin) != NULL) 
     {
          
          if (strlen(input) < 2)
               printf("$  ");
          else
          {

              cmd = parse(input);

              /* few error checking conditions */
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
