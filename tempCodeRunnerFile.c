#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for fork()
int delay, pid;
int main(void) {
 while (1) {
    printf("Enter alarm delay: ");
    scanf("%d", &delay);
    pid = fork();
    if (pid == 0) { // child process
        sleep(delay);
        printf("ALARM from pid %d!\a\n", getpid());
        exit(0);
    } else { // parent process
        printf("New child: %d\n", pid);
    }
 }
}