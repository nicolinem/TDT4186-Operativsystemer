#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

#ifndef MAX_BUF
#define MAX_BUF 500
#endif

int batchMode = 0;
int breakString(char *str);
char path[MAX_BUF];
char *input = NULL;
size_t input_buffer_size = 0;
int StdIn, StdOut = -1;

/* Declaration for parseString() */
int i;
char *token;
char *array[512];
char **user_command = array;

void resetIO(int fd, int type)
{
    int a = dup2(fd, type);
}

void resetStdIO()
{
    if (StdIn != -1)
        resetIO(StdIn, 0);
    if (StdOut != -1)
        resetIO(StdOut, 1);
    StdIn = -1;
    StdOut = -1;
}

void tokenize_string(char *input)
{
    i = 0;
    token = strtok(input, "\n ");
    while (token != NULL)
    {
        array[i++] = token;
        token = strtok(NULL, "\n ");
    }
    array[i] = NULL;
}

int findSymbol(char **argv, char *sym)
{
    int pos;
    // return pos of symbol to caller
    for (pos = 0; argv[pos] != NULL; pos++)
        if (strcmp(argv[pos], sym) == 0)
            return pos;
    return -1; // nothing was found
}

int checkRedirectionIO(char *command)
{
    char *out = strstr(command, ">");
    char *in = strstr(command, "<");

    if ((out != NULL) && (in != NULL))
    {
        // both inut and output redirection
        return 3;
    }
    else if (out != NULL)
    {
        // output redirection only
        return 2;
    }
    else if (in != NULL)
    {
        // input redirection only
        return 1;
    }
    else
    {
        return -1;
    }
}

int execute()
{

    pid_t pid = fork(); // Create a new process
    if (pid == 0)
    { // If not successfully completed
        if (execvp(array[0], array) == -1)
        {                            // If returned -1 => something went wrong! If not then command successfully completed */
            perror("Wrong command"); // Display error message
            exit(errno);
        }
    }
    else
    {
        resetStdIO();
        int status;

        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);
            printf("Exit status of the child was %d\n",
                   exit_status);
        }

        return 0;
    }
}

int changeDirectory()
{
    // If we write no path (only 'cd'), then go to the home directory
    if (array[1] == NULL)
    {
        chdir(getenv("HOME"));
        return 1;
    }
    // Else we change the directory to the one specified by the
    // argument, if possible
    else
    {
        if (chdir(array[1]) == -1)
        {
            printf(" %s: no such directory\n", array[1]);
            return -1;
        }
    }
    return 0;
}

// checks for redirection and returns the case
// int redirectionCheck(char *command)
// {
//     char *out = strstr(command, ">");
//     char *in = strstr(command, "<");

//     if ((out != NULL) && (in != NULL))
//     {
//         // both inut and output redirection
//         return 3;
//     }
//     else if (out != NULL)
//     {
//         // output redirection only
//         return 2;
//     }
//     else if (in != NULL)
//     {
//         // input redirection only
//         return 1;
//     }
//     else
//     {
//         return -1;
//     }
// }

int redirect(char *redirectTo, int type)
{
    int fd;
    int fdToReplace;
    if (type == 0)
    {
        fd = open(redirectTo, O_CREAT | O_RDWR, 0644);
        fdToReplace = 0;
    }
    else
    {
        fd = open(redirectTo, O_CREAT | O_RDWR, 0644);
        fdToReplace = 1;
    }
    int ret = dup(fdToReplace);
    dup2(fd, fdToReplace);
    return ret;
}

/* Finds and returns the index of "<" and ">" in the command prompt */
int find_redir_index(char **command, int type)
{
    char sign[2];
    if (type == 0)
        strcpy(sign, "<");
    else
        strcpy(sign, ">");
    int index = 0;
    while (*command)
    {
        if (strcmp(*command, sign) == 0)
            return index;
        index++;
        command++;
    }
    return -1;
}

/* Set stdin and stout if user specifies, and returns command without  "<" and ">" */
char **handle_redir(char **cmd)
{
    int redirInput = find_redir_index(cmd, 0);  // index of the input
    int redirOutput = find_redir_index(cmd, 1); // index of the outputr
    if (redirInput != -1)
    {
        StdIn = redirect(cmd[redirInput + 1], 0);
        cmd[redirInput] = 0;
    }

    if (redirOutput != -1)
    {
        StdOut = redirect(cmd[redirOutput + 1], 1);
        cmd[redirOutput] = 0;
    }
    return cmd;
}

int main()
{
    char cwd[4000];
    int status;

    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd() error\n");
        }

        printf("%s: ", cwd);

        /* Get user command input */
        getline(&input, &input_buffer_size, stdin);

        /* Check if input is empty */
        if (strcmp(input, "\n") == 0)
        {
            perror("Please type something:) ");
            continue;
        }

        tokenize_string(input);

        user_command = handle_redir(user_command);

        if (strcmp(array[0], "cd") == 0)
        {
            changeDirectory();
        }
        else
        {
            execute();
        }
        resetStdIO();
    }
}