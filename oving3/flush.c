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

void reset_IO();
void reset_std_IO();
void tokenize_string(char *input);
int find_char(char **argv, char *sym);
int execute();
int change_directory();
int redirect(char *redirect_target, int mode);
int find_redir_index(char **command, int type);
char **handle_redir(char **cmd);
void check_tasks();
void check_task_status(int status, char *input);
void remove_task(struct task *task);
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

        check_tasks();

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

        // background = is_background(input);

        // printf("Background: %d\n", background);

        user_command = handle_redir(user_command);
        // If user presses ctrl-d and thus no inputs are given.
        if (array[0] == NULL) {
            
            printf("Exiting program \n");
            return 0;
        }

        if (strcmp(array[0], "cd") == 0)
        {
            change_directory();
        }
        if (strcmp(array[0], "jobs") == 0)
        {
            print_nodes();
        }
        else
        {
            execute();
        }
        reset_std_IO();
    }
}

/**
 * Resets to standard input and output, used after redirecting IO
 */
void reset_std_IO()
{
    if (StdIn != -1)
        dup2(StdIn, 0);

    if (StdOut != -1)
        dup2(StdOut, 1);
    StdIn = -1;
    StdOut = -1;
}

/**
 * Splits user input into tokens so command can be parsed
 */
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

/**
 * @brief used to locate symbol
 * @param aargv string to search
 * @param sym symbol to be located
 * @returns index of symbol, -1 if not found
 */
int find_char(char **argv, char *sym)
{
    int pos;
    for (pos = 0; argv[pos] != NULL; pos++)
        if (strcmp(argv[pos], sym) == 0)
            return pos;
    return -1;
}

/**
 * @brief creates task struct and adds to linked list of backround tasks
 * @param pid id of child process
 * @param cmd command from user input
 */
void add_task(int pid, char *cmd)
{
    struct task *new_task = (struct task *)malloc(sizeof(struct task));
    new_task->pid = pid;
    strcpy(new_task->cmd, cmd);

    /* set next pointer */
    new_task->next = NULL;
    if (head == NULL)
    {
        head = new_task;
    }

    /* set prev pointer */
    new_task->prev = tail;
    if (tail != NULL)
        tail->next = new_task;
    tail = new_task;
}

/**
 * @brief removes task from linked list, used after checking if backround process is completed
 * @param task task to remove
 */
void remove_task(struct task *task)
{
    // printf("Removing task from linked list\n");
    /* check for first node */
    if (task->prev != NULL)
    {
        task->prev->next = task->next;
    }
    else
    {
        head = task->next;
    }

    /* check for last node */
    if (task->next != NULL)
    {
        task->next->prev = task->prev;
    }
    else
    {
        tail = task->prev;
    }
    free(task);
}

/**
 * @brief prints status of exited task
 * @param status int from waitpid
 * @param input executed task
 */
void check_task_status(int status, char *input)
{
    // printf("Checking input status \n");
    /* removes newline from input string */
    input[strcspn(input, "\n")] = 0;

    if (WIFEXITED(status))
    {
        int exit_status = WEXITSTATUS(status);
        printf("Exit status for task: |%s| = %d\n", input, exit_status);
    }
}

/**
 * @brief checks if backround processes has completed
 */
void check_tasks()
{
    struct task *t = head;
    while (t != NULL)
    {
        int status;

        if (waitpid(t->pid, &status, WNOHANG))
        {
            check_task_status(status, t->cmd);

            remove_task(t);
        }
        t = t->next;
    }
}

/**
 * @brief executes commands in child processes
 * Checks if tasks is backround task, if so does not wait for process to complete before new prompt
 */
int execute()
{
    int background = is_background(user_command);

    pid_t pid = fork();

    if (pid == 0)
    {
        if (execvp(array[0], array) == -1)
        { // If returned -1 => something went wrong! If not then command successfully completed */
            perror("Wrong command");
            exit(errno);
        }
    }
    else
    {
        reset_std_IO();

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

/**
 * @brief Internal command for changing directory
 * If no path is specifies we direct to HOME directory
 * */
int change_directory()
{
    if (array[1] == NULL)
    {
        chdir(getenv("HOME"));
        return 1;
    }
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
 * @brief Prints all tasks in list,
 * displays background tasks with prompt "jobs"
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

/**
 * @brief checks if user input contains "&"
 * @param aargs input to be checked
 * @returns true if user input contains "&"
 */
int is_background(char **args)
{
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

/**
 * @brief redirects IO
 * @param redirect_target target for new IO source
 * @param mode if 0
 */
int redirect(char *redirect_target, int mode)
{
    int old_fd;

    int fd = open(redirect_target, O_CREAT | O_RDWR, 0644);

    int copy_mode = dup(mode);
    dup2(fd, mode);
    return copy_mode;
}

/**
 * @brief Finds and returns the index of "<" and ">" in the command prompt
 * @param command input to be checked
 * @param type decide if we are searching for "<" or ">"
 * @returns index of redir symbol, -1 if not found
 */
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

/**
 * @brief Redirect stdin and stout if user specifies, and returns command without  "<" and ">"
 * @param cmd user input to be checked for redirection
 * @returns user input but with "<" and ">" removed if found
 */
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
