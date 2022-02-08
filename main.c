#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>


int year, month, day, hour, minute, second, countdownTime, pid;
struct tm tid = {0};

char input;
time_t t;   
time_t alarms[10] = {0};


int main()
{
    while (1){
    time(&t);
    printf("\nWelcome to the alarm clock! It is currently: %s and the pid is %d", ctime(&t), getpid());
    printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel), 'x' (exit)");
    scanf(" %c", &input);
    
    if (input == 's'){
        printf("Schedule alarm at which date and time?");
        scanf("%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute ,&second); // Input på format: yyyy-mm-dd hh:mm:ss
        tid.tm_year = year - 1900;
        tid.tm_mon = month -1;
        tid.tm_mday = day;
        tid.tm_hour = hour;
        tid.tm_min = minute;
        tid.tm_sec = second;

        time_t convertedTime;
        convertedTime = mktime(&tid); // Gjør om input strengen til tidsformat

        countdownTime = difftime(convertedTime, t);
        printf("Delay:  %d\n", countdownTime);

        pid = fork();               // Lager en barneprosess med fork() (duplikat av koden, i samme sted, så skal den termineres), det er denne som skal telle ned i bakrunnen
        if (pid == 0) {              // Kode som kun kjører dersom vi er i en barneprosess
            sleep(countdownTime);
            printf("Ding!, alarm for %d went off", getpid());
            exit(0);
        } else{                                         // Foreldreprosessen                      
            printf("Nytt barn: %d\n", pid);             //For testing, printer ut barneprosessens ID-nummer, så vet vi hvilken klokke som ringer
        }

        }


    // else if (input == 'l'){
    //    for (int i = 0; i < sizeof(alarms); i++) {
    //         printf("the time is %s", time(&alarms[i]));      
    //     }   
    // }       
    // else if (input == 'c'){
       
    // }
    // else if (input == 'x'){
    //     printf("Goodbye!");
    // }
    else {
        printf("Invalid command");
    }
    
    /*printf("Enter a number: ");
    scanf("%f", &num1);
    printf("Enter another number: ");
    scanf("%lf", &num2);

    printf("num1 = %f\n", num1);
    printf("num2 = %lf", num2);*/

    }
    exit(0);
}