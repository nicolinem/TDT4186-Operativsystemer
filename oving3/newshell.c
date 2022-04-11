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

struct task
{
    int pid;
    char cmd[MAX_BUF];
    struct task *prev;
    struct task *next;
};

char *input = NULL;
size_t input_buffer_size = 0;
int StdIn, StdOut = -1;
// int background;

/* Declaration for tokenize_string() */
int i;
char *token;
char *array[512];
char **user_command = array;

struct task *head = NULL;
struct task *tail = NULL;

void resetIO();
void resetStdIO();
void tokenize_string(char *input);
int findSymbol(char **argv, char *sym);
int execute();
int changeDirectory();
int redirect(char *redirectTo, int type);
int find_redir_index(char **command, int type);
char **handle_redir(char **cmd);
void check_nodes();
void check_status(int status, char *input);
void remove_task(struct task *remove);
void add_task(int pid, char *cmd);
int is_background(char **args);
void print_nodes();
// int check_if_background_task(char input[MAX_BUF]);

int main()
{
    char cwd[4000];
    int status;

    while (1)
    {

        check_nodes();

        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd() error\n");
        }

        printf("%s:", cwd);

        /* Get user command input */
        getline(&input, &input_buffer_size, stdin);

        /* Check if input is empty */
        if (strcmp(input, "\n") == 0)
        {
            perror("Please type something:) ");
            continue;
        }

        tokenize_string(input);

        // background = is_background(input);

        // printf("Background: %d\n", background);

        user_command = handle_redir(user_command);

        if (strcmp(array[0], "cd") == 0)
        {
            changeDirectory();
        }
        if (strcmp(array[0], "jobs") == 0)
        {
            print_nodes();
        }
        else
        {
            execute();
        }
        resetStdIO();
    }
}

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

void add_task(int pid, char *cmd)
{
    struct task *new_task = (struct task *)malloc(sizeof(struct task));
    printf("Adding new background task: %d\n", pid);
    /* update values */
    new_task->pid = pid;
    strcpy(new_task->cmd, cmd);

    /* update next pointer */
    new_task->next = NULL;
    if (head == NULL)
    {
        head = new_task;
    }

    /* update previous pointer */
    new_task->prev = tail;
    if (tail != NULL)
        tail->next = new_task;
    tail = new_task;
}

void remove_task(struct task *remove)
{
    // printf("Removing task from linked list\n");
    /* checks if first node */
    if (remove->prev != NULL)
    {
        remove->prev->next = remove->next;
    }
    else
    {
        head = remove->next;
    }

    /* checks if last node */
    if (remove->next != NULL)
    {
        remove->next->prev = remove->prev;
    }
    else
    {
        tail = remove->prev;
    }
    free(remove);
}

/**
 * @brief Checks the status of the exited task and prints it.
 * @param status int from waitpid
 * @param input the task that has been executed.
 */
void check_status(int status, char *input)
{
    // printf("Checking input status \n");
    /* removes newline from input string */
    input[strcspn(input, "\n")] = 0;

    if (WIFEXITED(status))
    {
        int es = WEXITSTATUS(status);
        printf("Exit status [%s] = %d\n", input, es);
    }
}

void check_nodes()
{
    struct task *ptr = head;
    while (ptr != NULL)
    {
        int status;

        /* checks if processes are complete */
        if (waitpid(ptr->pid, &status, WNOHANG))
        {
            check_status(status, ptr->cmd);

            remove_task(ptr);
        }
        ptr = ptr->next;
    }
}

// int check_if_background_task(char input[MAX_LIMIT])
// {
//     /* removes newline from input string */
//     input[strcspn(input, "\n")] = 0;
//     int length = strlen(input) - 1;

//     /* checks for '&' and removes it from string */
//     int check = input[length] == '&';
//     if ((length > 0) && (input[length] == '&'))
//     {
//         input[length] = '\0';
//         length--;
//     }

//     /* removes trailing whitespaces after removing '&' */
//     while (length > -1)
//     {
//         if (input[length] == ' ' || input[length] == '\t')
//             length--;
//         else
//             break;
//         input[length + 1] = '\0';
//     }

//     return check;
// }

int execute()
{
    // printf("Executing command");
    int background;

    background = is_background(user_command);
    // printf("Cmd %s\n", array[0]);

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
        if (background)
        {
            add_task(pid, input);
        }
        else
        {

            int status;

            waitpid(pid, &status, 0);

            if (WIFEXITED(status))
            {
                int exit_status = WEXITSTATUS(status);
                printf("Exit status of the child was %d\n",
                       exit_status);
            }
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

/**
 * @brief Prints all nodes in the linked list,
 * is called by the prompt "jobs"
 */
void print_nodes()
{
    struct task *ptr = head;
    while (ptr != NULL)
    {
        printf("[pid %d] %s\n", ptr->pid, ptr->cmd);
        ptr = ptr->next;
    }
}

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

int is_background(char **args)
{
    // /* removes newline from input string */
    // input[strcspn(input, "\n")] = 0;
    // int length = strlen(input) - 1;

    // /* checks for '&' and removes it from string */
    // int check = input[length] == '&';
    // if ((length > 0) && (input[length] == '&'))
    // {
    //     printf("found &");
    //     input[length] = '\0';
    //     length--;
    // }
    // printf("found no&");

    // /* removes trailing whitespaces after removing '&' */
    // while (length > -1)
    // {
    //     if (input[length] == ' ' || input[length] == '\t')
    //         length--;
    //     else
    //         break;
    //     input[length + 1] = '\0';
    // }

    // return check;

    // Current position in array
    int last_arg = 0;

    // printf("hello %s\n", *args);
    // printf("%s", *args);
    // printf("%s", **args);

    // Finding last arg in array
    while (args[last_arg + 1] != NULL)
    {
        // printf("looking for &\n");
        // printf("looking for %s\n", args[last_arg]);
        last_arg += 1;
    }

    // Checking if task is background`
    if (strcmp(args[last_arg], "&") == 0)
    {
        // Remove '&' token for future executing
        args[last_arg] = NULL;
        // printf("yes we found an &\n");
        // Return true
        return 1;
    }
    // printf("no found an &\n");

    // Return false if: '&' wasn't founded
    return 0;
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
