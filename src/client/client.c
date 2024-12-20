#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER1_PORT 8080
#define SERVER2_PORT 8081
#define BUFFER_SIZE 1024

void connect_to_server(int port, int *sock);
void disconnect_from_server(int *sock);
void send_command(int sock, char *command);

int sock1 = -1;
int sock2 = -1;

int main() {
    char choice;
    printf("\nChoice command\n");
    printf("1. Connect server 1\n");
    printf("2. Connect server 2\n");
    printf("3. Disconnect server 1\n");
    printf("4. Disconnect server 2\n");
    printf("5. Get last error code\n");
    printf("6. Get cursor position\n");
    printf("7. Percentage of pthysical memory used\n");
    printf("8. Percentage of virtual memory used\n");
    printf("q. Exit\n");
    printf("Command: ");

    while (1) {

        
        scanf("%c", &choice);

        switch (choice) {
            case '1':
                connect_to_server(SERVER1_PORT, &sock1);
                break;
            case '2':
                connect_to_server(SERVER2_PORT, &sock2);
                break;
            case '3':
                disconnect_from_server(&sock1);
                break;
            case '4':
                disconnect_from_server(&sock2);
                break;
            case '5':
                send_command(sock1, "error");
                break;
            case '6':
                send_command(sock1, "cursor");
                break;
            case '7':
                send_command(sock2, "physical");
                break;
            case '8':
                send_command(sock2, "virtual");
                break;
            case 'q':
                disconnect_from_server(&sock1);
                disconnect_from_server(&sock2);
                exit(0);
            default:
                // printf("Invalid input\n");
                break;
        }
    }

    return 0;
}

void connect_to_server(int port, int *sock) {
    struct sockaddr_in serv_addr;

    if (*sock != -1) {
        printf("Already connected %d\n", port);
        return;
    }
    
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket create error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInet_pton error\n");
        return;
    }

    if (connect(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection error %d\n", port);
        close(*sock);
        *sock = -1;
        return;
    }

    char buffer[BUFFER_SIZE] = {0};
    read(*sock, buffer, BUFFER_SIZE);
    printf("%s", buffer);
}

void disconnect_from_server(int *sock) {
    if (*sock != -1) {
        close(*sock);
        printf("Disconnected\n");
        *sock = -1;
    } else {
        printf("Already disconnected\n");
    }
}

void send_command(int sock, char *command) {
    if (sock != -1) {
        send(sock, command, strlen(command) + 1, 0);
        char buffer[BUFFER_SIZE];
        if (read(sock, buffer, BUFFER_SIZE)) {
            printf("%s\n", buffer);
        } else {
            printf("Read error\n");
        }
    } 
    else {
        printf("No connection\n");
    }
}