#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

float num1;
double num2;

int year, month, day, hour, minute, second, countdownTime, pid, st;
struct tm tid = {0};

char input;
time_t t;
int delay, pid;
time_t alarms[10] = {0};
int pids[10] = {0};
int pos, size, i;
int childp;
double counter = 0;

volatile sig_atomic_t shutdown_flag = 1;

void cleanupRoutine(int signal_number)
{
    // shutdown_flag = 0;
    int saved_errno = errno;
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0)
    {
    }
    errno = saved_errno;
}

int main()
{
    struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    sigterm_action.sa_handler = &cleanupRoutine;

    sigterm_action.sa_flags = 0;
    sigemptyset(&sigterm_action.sa_mask);
    sigterm_action.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sigterm_action, 0) == -1)
    {
        perror(0);
        exit(1);
    }
    while (1)
    {

        time(&t);
        printf("\nWelcome to the alarm clock! It is currently: %s", ctime(&t));
        printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel), 'x' (exit)");
        scanf("\n%c", &input);
        fflush(stdin);

        if (input == 's')
        {
            printf("Schedule alarm at which date and time?");

            if (counter == 10)
            {
                printf("\nNo more available alarms!\n");
                continue;
            }

            scanf("\n%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second); // input format: yyyy-mm-dd hh:mm:ss
            tid.tm_year = year - 1900;
            tid.tm_mon = month - 1;
            tid.tm_mday = day;
            tid.tm_hour = hour;
            tid.tm_min = minute;
            tid.tm_sec = second;

            time_t convertedTime;

            convertedTime = mktime(&tid); // convert input to time 

            for (i = 0; i < 10; i++)
            {
                if (alarms[i] == 0)
                {
                    alarms[i] = convertedTime;
                    counter++;
                    break;
                }
            }

            countdownTime = difftime(convertedTime, t); // time from now to alarm

            if (countdownTime < 0)
            {
                printf("Cannot set alarm in the past!\n");
                continue;
            }

            pid = fork(); // create child process
            if (pid == 0) 
            {
                sleep(countdownTime);
                printf("\nDing!, alarm for %d went off\n", getpid());
                system("afplay --volume 0.2 alarm.mp3");
                exit(0);
            }
            else
            {
                for (i = 0; i < 10; i++)
                {
                    if (pids[i] == 0)
                    {
                        pids[i] = pid;
                        break;
                    }

                }                                                            
                printf("Alarm %d set for %d seconds\n", pid, countdownTime); 
            }
        }

        else if (input == 'l')
        {
            if (counter == 0)
            {
                printf("No scheduled alarms\n");
                continue;
            }
            for (i = 0; i < 10; i++)
            {
                if (alarms[i] != 0)
                {
                    printf("Alarm %d at %s\n", i + 1, ctime(&alarms[i]));
                }
            }
        }

        else if (input == 'c')
        {
            if (counter == 0)
            {
                printf("No scheduled alarms\n");
                continue;
            }
            printf("Cancel which alarm?\n");
            scanf("\n%d", &pos);

            if (pos < 0 || pos > 10)
            {
                printf("Invalid position. Enter a position between  1 to 10\n");
            }
            if (alarms[pos - 1] == 0)
            {
                printf("No such alarm!\n");
            }
            else
            {
                alarms[pos - 1] = 0;
                pids[pos - 1] = 0;
                kill(pid, SIGKILL);
                counter--;
            }
        }
        else if (input == 'x')
        {
            printf("Goodbye!\n");
            exit(0);
        }
        else
        {

            printf("Invalid command\n");
        }
    }
    exit(0);
}