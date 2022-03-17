// Server side C program to demonstrate Socket programming

#include <stdio.h>

#include <sys/socket.h>

#include <unistd.h>

#include <stdlib.h>

#include <netinet/in.h>

#include <string.h>

#define PORT 8080

int main(int argc, char const *argv[])

{

int server_fd, new_socket;

long valread;

struct sockaddr_in address;

int addrlen = sizeof(address);

char buffer[50]; // Buffer to store data

FILE * stream;

stream = fopen("index.html", "r");

int count = fread(&buffer, sizeof(char), 30, stream);

fclose(stream);

char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: count\n";

if(c.r_type == "GET")

{

ifstream fp;

char *readblock;

size_t size;

fp.open(c.r_fp.c_str());

if (send(c.r_acceptid,c.r_ctype.c_str(), strlen(c.r_ctype.c_str()), 0) == -1)

perror("send");

if (fp.is_open())

{

fp.seekg (0, ios::end);

size = fp.tellg();

readblock = new char [size];

fp.seekg (0, ios::beg);

fp.read(readblock, size);

}

else

cout<<"Never went Inside"<<endl;

if (send(c.r_acceptid, readblock, size, 0) == -1)

perror("send");

fp.close();

delete [] readblock;

close(c.r_acceptid);

}

// Creating socket file descriptor

if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)

{

perror("In socket");

exit(EXIT_FAILURE);

}

address.sin_family = AF_INET;

address.sin_addr.s_addr = INADDR_ANY;

address.sin_port = htons(PORT);

memset(address.sin_zero, '\0', sizeof address.sin_zero);

if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)

{

perror("In bind");

exit(EXIT_FAILURE);

}

if (listen(server_fd, 10) < 0)

{

perror("In listen");

exit(EXIT_FAILURE);

}

while (1)

{

printf("\n+++++++ Waiting for new connection ++++++++\n\n");

if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)

{

perror("In accept");

exit(EXIT_FAILURE);

}

char buffer[30000] = {0};

valread = read(new_socket, buffer, 30000);

printf("%s\n", buffer);

write(new_socket, hello, strlen(hello));

printf("------------------Hello message sent-------------------\n");

close(new_socket);

}

return 0;

}