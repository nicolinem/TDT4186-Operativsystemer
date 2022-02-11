#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

//     void signalHandler(int signal)
//     {   
//     if (signal == SIGALRM)
//     {
//     printf("Ding!\n");
//     wait(NULL);
//     }

// }
float num1;
    double num2;

    int year, month, day, hour, minute, second;
    struct tm tid = {0};

    char input;
    time_t t;   
    int delay, pid;
    time_t alarms[10] = {0};
    int pos,size,i;

int main()
{
    while (1){
    time(&t);
    printf("\nWelcome to the alarm clock! It is currently: %s", ctime(&t));
    printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel), 'x' (exit)");
    scanf("%c", &input);
    
    if (input == 's'){
        printf("Schedule alarm at which date and time?");
        scanf("%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute ,&second); // Inpput på format: yyyy-mm-dd hh:mm:ss
        tid.tm_year = year - 1900;
        tid.tm_mon = month -1;
        tid.tm_mday = day;
        tid.tm_hour = hour;
        tid.tm_min = minute;
        tid.tm_sec = second;

        time_t convertedTime;
        convertedTime = mktime(&tid); // Gjør om input strengen til tidsformat

        delay = difftime(convertedTime, t); // Tid fra nå til alarm
        printf("Delay:  %d\n", delay);

        pid = fork();               // Lager en barneprosess med fork() (duplikat av koden, i samme sted, så skal den termineres), det er denne som skal telle ned i bakrunnen
        if (pid == 0)               // Barneprosessen
        {
            sleep(delay);
            printf("Ding!, alarm went off");
            exit(0);
        } else{                     // Foreldreprosessen - MEN DEN FORTSETTER IKKE SOM DEN SKAL! Den burde restarte med en gang og spørre om ny input, nå venter den til barnet er ferdig
            wait(NULL);                         //Se tempCodeRunnerFile.c for enklere men fungerende kode
            printf("test: %d\n", pid);          //For testing, printer ut barneprosessens ID-nummer, så vet vi hvilken klokke som ringer
        }
        

        printf("done!!\n"); //For testing bare

        }

    else if (input == 'l'){
        for (int i = 0; i < sizeof(alarms); i++) {
             printf("the time is %s", time(&alarms[i]));      
         }   
     }       
     else if (input == 'c'){
        printf("Cancel which alarm?");
        scanf("%d", &pos);
        int size = sizeof(alarms)/sizeof(alarms[0]);

        if(pos < 0 || pos > size){
            printf("Invalid position. Enter a position between  1 to %d", size);
        }
        else{
            for(i=pos-1; i<pos-1; i++){
                alarms[i] = alarms[i+1];
            }

            size--;
        }
        return 0;
      
       
     }


     else if (input == 'x'){
         printf("Goodbye!");
     }
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
}