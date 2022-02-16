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

int year, month, day, hour, minute, second, countdownTime, pid;
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
    scanf("\n%c", &input);
    
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
        alarms[0] = convertedTime;

        countdownTime = difftime(convertedTime, t); // Tid fra nå til alarm

        pid = fork();               // Lager en barneprosess med fork() (duplikat av koden, i samme sted, så skal den termineres), det er denne som skal telle ned i bakrunnen
        if (pid == 0)               // Barneprosessen
        {
            sleep(countdownTime);
            printf("\nDing!, alarm for %d went off\n", getpid());
            exit(0);
        } else{                                  //For testing, printer ut barneprosessens ID-nummer, så vet vi hvilken klokke som ringer
            printf("Alarm %d set for %d seconds\n", pid, countdownTime);          //For testing, printer ut barneprosessens ID-nummer, så vet vi hvilken klokke som ringer
        }
        


        }

    else if (input == 'l'){
        for (int i = 0; i < sizeof(alarms); i++) {
             printf("the time is %s", time(&alarms[i]));      
         }   
     }       
     
    // else if (input == 'l'){
    //    for (int i = 0; i < sizeof(alarms); i++) {
    //         printf("the time is %s", time(&alarms[i]));      
    //     }   
    // }       
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

            printf("\nElements of array after delete are:  ");
            for(i=0; i<size; i++) {
                printf("%d\t", alarms[i]);
            }
        }
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
    exit(0); //denne kjører mange ganger
}