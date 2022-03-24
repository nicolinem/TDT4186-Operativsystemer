#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>
#include "bbuffer.h"

#define PORT 8000
#define BUFSIZE 4096
#define THREAD_POOL_SIZE 10

pthread_t thread_pool[THREAD_POOL_SIZE];

// BNDBUF *bbuffer = malloc(sizeof(BNDBUF));
BNDBUF *bbuffer;

void *handle_connection(int client_sock);
void read_file_send_response(char *filename, char *cwd, int client_socket);
void *thread_function(void *arg);

int main(void)
{
    bbuffer = bb_init(10);

    printf("Hello world!");
    int socket_desc, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_size;

    // Vi lager threads som skal håndtere requestene, de henter requests fra bufferen ( i teorien)
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        pthread_create(&thread_pool[i], NULL, thread_function, (void *)NULL);
    }

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Error binding to port\n");
        return -1;
    }
    printf("Success: Bind to port\n");

    // Listen for clients:
    if (listen(socket_desc, 1) < 0)
    {
        printf("Error while listening\n");
        return -1;
    }
    printf("\nListening for incoming connections.....\n");

    while (1)
    {
        // Accept an incoming connection:
        client_size = sizeof(client_addr);

        int client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
        if (client_sock < 0)
        {
            printf("Can't accept\n");
            return -1;
        }

        // Her legger vi til de innkommende requestene i bufferen ( i teorien)
        int *pclient = malloc(sizeof(int));
        *pclient = client_sock;
        bb_add(bbuffer, client_sock);

        printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    };
    close(socket_desc);

    return 0;
}

// Hva hver thread gjør, de venter basicly på å få en request fra bufferen
void *thread_function(void *arg)
{
    while (1)
    {

        int pclient = bb_get(bbuffer);
        if (pclient != 0)
        {
            handle_connection(pclient);
        }
    }
}

void *handle_connection(int client_sock)
{
    // int client_sock = *((int *)p_client_sock);
    // free(p_client_sock);

    char *cwd;          // path name for current directory
    char tmp_cwd[4000]; // temporary current working directory?
    if (getcwd(tmp_cwd, sizeof(tmp_cwd)) != NULL)
    {
        printf("Det funker!\n");
        cwd = strcat(tmp_cwd, "/doc");
    }
    else
        return NULL;

    char server_message[2000], client_message[2000];

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0)
    {
        printf("Couldn't receive\n");
        return NULL;
    }

    printf("Trying to access more memory\n");
    char request[3][4096];

    char delim[] = " ";
    char *token = strtok(client_message, delim);

    int tokenPossition = 0;

    while (token != NULL)
    {
        printf("Trying to input to memmory\n");
        if (tokenPossition < 4)
        {
            strcpy(request[tokenPossition], token);
        }
        printf("%s\n", token);
        token = strtok(NULL, delim);
        tokenPossition++;
    }

    read_file_send_response(request[1], cwd, client_sock);

    printf("While loop finished\n");

    printf("Token first possition: %s\n", request[0]);
    printf("Token path: %s \n", request[1]);

    close(client_sock);
    return NULL;
}

// Håndterer hver request og svarer serveren
void read_file_send_response(char *filename, char *cwd, int client_socket)
{
    char buffer[BUFSIZE];
    size_t bytes_count;
    int msgsize = 0;
    char actualpath[512] = {0};

    strcpy(actualpath, cwd);
    strncat(actualpath, filename, 512 - strlen(actualpath));
    printf("%s\n", actualpath);

    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL)
    {
        printf("ERROR opening: %s\n", filename);
    }

    while ((bytes_count = fread(buffer, 1, BUFSIZE, fp)) > 0)
    {
        printf("sending %zu bytes\n", bytes_count);
        printf("sending %s\n", buffer);

        // Create response message to send to server, gives header to file content
        char response[BUFSIZE];
        memset(response, 0, BUFSIZE);
        strcat(response, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ");
        char size[256];
        sprintf(size, "%ld", sizeof(buffer));
        strcat(response, "\n\n");
        strcat(response, buffer);

        if (send(client_socket, response, sizeof(response), 0) < 0)
        {
            printf("Error sending response\n");
            exit(1);
        }
    }
}