#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 1111
#define BUFFER_SIZE 256

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Создание сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Преобразование IP-адреса в бинарный формат
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(1);
    }

    // Соединение с сервером
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(1);
    }

    printf("Connected to server...\n");

    // Отправка сообщения
    printf("Enter message to send (or 'exit' to quit): ");
    fgets(buffer, BUFFER_SIZE, stdin);

    if (strcmp(buffer, "exit\n") != 0) {
        send(sock, buffer, strlen(buffer), 0);
        recv(sock, buffer, BUFFER_SIZE, 0);
        printf("Server response: %s\n", buffer);
    }

    close(sock);
    return 0;
}