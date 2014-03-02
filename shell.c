/* Group Members : Alexander DeOliveira, Elana Braff, Jesse Huang */
#include "shell.h"
#define NUM_COMMANDS 2

/* stores the built in function table */
struct builtins functionTable[NUM_COMMANDS];

/* takes in pointer to user input,
   and pointer to cmd (assumes it is already allocated) */
command *parse(char *input) 
{
     int count, arg = 0, start = 0,quote = 0;
     command *cmd, *head;
     
     if (!(cmd = (command *)malloc(sizeof(struct command)))) 
     {
          printf("bad memory allocation\n");
          exit(EXIT_FAILURE);
     }
     head = cmd;
     memset(cmd->argv, 0, 51);
     /* get rid of leading spaces and tabs */
     while (*input == ' ' || *input == '\t')
          input++;
     cmd->argv[0] = input;
     cmd->argc = 1;
     cmd->next = NULL;
     cmd->head = head;
     
     if (input[strlen(input)-1] == '\n') 
     {
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
          case '\t':
               if (!quote)  /* found a space not inside of a quote */
               {
                    input[count] = '\0';
                    
                    /* remove extra spaces */
                    while(input[count + 1] == ' ' || input[count + 1] == '\t')
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
                    if (!(cmd->next = (command *)malloc(sizeof(struct command)))) 
                    {
                         fprintf(stderr, "bad memory allocation\n");
                         exit(EXIT_FAILURE);
                    }
                    cmd = cmd->next;
                    cmd->argc = 1;
                    arg=0;
                    memset(cmd->argv, 0, 51);
                    cmd->head = head;
                    
                    /* remove extra spaces */
                    while(input[count + 1] == ' ' || input[count + 1] == '\t')
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

/* counts the number of arguments for a command */
int getNumArgs(command *cmd) 
{
     return cmd->argc - 1;
}

/* executes the built in cd function */
int cd_cmd(int argc, char **argv) 
{
     if (argc == 1)
     {
          chdir(getenv("HOME"));
     }
     else if (argc == 2) 
     {
          if(chdir(argv[1]) == -1)    
               perror("cd");
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

/* executes the built in exit function */
int exit_cmd(int argc, char **argv) 
{
     int exitCode = 0;
     if (argc == 2)
     {
          exitCode = atoi(argv[1]);
     }
     else if (argc > 2)
     {
          fprintf(stderr, "exit: too many arguments\n");
          return 1;
     }
     
     exit(exitCode);
}

/* Initializes the function table */
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
     while(cmd!=NULL)
     {
          command *temp = cmd;
          cmd=cmd->next;
          free(temp);
     }     
}

/* Debug function for printing out the command structures */
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
               (*(functionTable[i].f))(getNumArgs(cmd), cmd->argv);
               return 1;
          }
     }
     return 0;
}

/* Remove quotes from the argument list */
void stripQuotes(command *cmd)
{
     int i, j, len, quoteCount = 0;
     char quote;
     while(cmd!=NULL)
     {
          for(i = 0; i < cmd->argc - 1; i++)
          {
               len = strlen(cmd->argv[i]);
               for (j = 0; j < len; j++)
               {
                    if ((cmd->argv[i][j] == '\"' || cmd->argv[i][j] == '\'') && quoteCount == 0)
                    {
                         quote = cmd->argv[i][j];
                         cmd->argv[i]++;
                         quoteCount++;
                    } 
                    else if (quoteCount == 1 && cmd->argv[i][j] == quote)
                    {
                         cmd->argv[i][j] = '\0';
                         quoteCount--;
                    }
               }
          }
          cmd=cmd->next;
     }
}

/* helper function for process it will handle all piping */
void processPipe(command *cmd, char **argv) 
{
     int fd[2];                 /* file descriptor for pipe */
     int pid;                   /* process id */
     int fd_in = 0;             /* input file descriptor */
     int status;                /* stores exit status */
     
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
               perror(argv[0]);
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
void process(command *cmd, char **argv) 
{
     /* check for built in commands*/
     if (!isBuiltIn(cmd))
     {
          processPipe(cmd, argv);
     }     
     
}

/* check if command is empty */
int isEmpty(char *input)
{
     int i, len = strlen(input);
     if (len < 2)
          return 1;
     for (i = 0; i < len-1; i++)
     {
          if (input[i] != ' ' && input[i] != '\t')
               return 0;
     }
     
     return 1;
}

/* main function, everything starts here */
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
          
          if (isEmpty(input))
               printf("$  ");
          else
          {
               cmd = parse(input);
               stripQuotes(cmd);
               
               /* few error checking conditions */
               if(cmd && cmd->argc == 0)
               {
                    freeCmd(cmd->head);
                    if (isatty(0))
                         printf("$  ");
                    continue;
               }
               else if (cmd)
                    process(cmd, argv);
               else if (isatty(0))
                    printf("$  Invalid Command!!!!\n");
               else
                    printf("  Invalid Command!!!!\n");
               
               freeCmd(cmd->head);
               
               if (isatty(0))
                    printf("$  ");
          }
     }     
     return 0;
}
