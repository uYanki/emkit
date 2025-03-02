#include <stdio.h>
#include <string.h>  //strlen
#include <sys/socket.h>
#include <arpa/inet.h>  //inet_addr

#define MAX_PAYLOAD_SIZE (512)

int main(int argc, char* argv[])
{
    int                socket_desc;
    struct sockaddr_in server;
    char               message[MAX_PAYLOAD_SIZE] = {0};

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = inet_addr("192.168.1.120");
    server.sin_family      = AF_INET;
    server.sin_port        = htons(333);

    // Connect to remote server
    if (connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        puts("connect error");
        return 1;
    }

    puts("Connected\n");

    // Send some data
    // uint8_t tmp[] = {0x1, 0x2, 0x3};
    // char tmp[] = "ABC";
    char tmp[] = "GET / HTTP/1.1\r\n\r\n";
    // strcpy(message, "1");
    // if( send(socket_desc , message , strlen(message) , 0) < 0)
    if (send(socket_desc, tmp, sizeof tmp, 0) < 0) {
        puts("Send failed");
        return 1;
    }
    puts("Data Send\n");

    return 0;
}
