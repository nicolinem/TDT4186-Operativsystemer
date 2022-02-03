#include <stdio.h>
#include <stdlib.h>
#include<time.h>
int main()
{
    float num1;
    double num2;

    int year, month, day, hour, minute, second;

    char input;
    time_t t;   
    int delay;
    time_t alarms[10] = {0};

    // int main(void) {
    // while (1) {
    // printf(“Enter alarm delay: “);
    // scanf(“%d”, &delay);
    // sleep(delay);
    // printf(“ALARM!!! \a\a\a”);
    // }

    //exit(0);
    time(&t);
    printf("\nWelcome to the alarm clock! It is currently: %s", ctime(&t));
    printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel), 'x' (exit)");
    scanf("%c", &input);
    
    if (input == 's'){
        printf("Schedule alarm at which date and time?");
        // sscanf("%4d.%2d.%2d %2d:%2d", &year, &month, &day, &hour, &minute);
        // printf(year, month);

        // scanf("%02d:%02d:%02d",&hour,&minute,&second);
    } 
    else if (input == 'l'){
       for (int i = 0; i < sizeof(alarms); i++) {
            printf("the time is %s", time(&alarms[i]));      
        }   
    }       
    else if (input == 'c'){
        time_t t2;
        printf("Cancel which alarm?");
        scanf("%d", &t2);
        int index = alarms.indexOf(t2);
        alarms.remove(index);
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

    return 0;
    
}