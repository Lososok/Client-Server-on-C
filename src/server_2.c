#include "server.h"

#define PORT 8081

int init_port(int *server_fd, int *opt, struct sockaddr_in *address);
void *session(void *arg);
void command_handler(char *command, int sock);
void *out(void *arg);
int physical_memory_used();
int virtual_memory_used();

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if (!init_port(&server_fd, &new_socket, &address)) { return 1; }

    initscr();
    printw("Run server 2\n");
    refresh();

    pthread_t check_cli;
    if (pthread_create(&check_cli, NULL, out, NULL)) {
        perror("Thread CLI create error");
        endwin();
        return 1;
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printw("Connect client %d on server 2\n", new_socket);
        refresh();

        pthread_t thread_id;
        int *new_sock = malloc(1);
        *new_sock = new_socket;

        if (pthread_create(&thread_id, NULL, session, (void *)new_sock) < 0) {
            perror("pthread create error");
            free(new_sock);
            continue;
        }

        pthread_detach(thread_id);
    }

    return 0;
}

int init_port(int *server_fd, int *opt, struct sockaddr_in *address) {
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return 0;
    }

    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, opt, sizeof(*opt))) {
        perror("setsockopt");
        return 0;
    }
    
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("bind failed");
        return 0;
    }

    if (listen(*server_fd, SOMAXCONN) < 0) {
        perror("listen");
        return 0;
    }
    return 1;
}

void *session(void *arg) {
    int sock = *(int *)arg;
    int read_size = 1;

    send(sock, "connect server 2\n", strlen("connect server 2\n") + 1, 0);

    while (read_size > 0) {
        char command_buffer[BUFFER_SIZE] = {0};
        read_size = read(sock, command_buffer, BUFFER_SIZE);
        command_handler(command_buffer, sock);
    }

    if (read_size == 0) {
        printw("client %d disconnect\n", sock);
        refresh();
    } else if (read_size == -1) {
        perror("read error");
    }

    free(arg);
    close(sock);
    return NULL;
}

void command_handler(char *command, int sock) {
    if (strcmp(command, "error") == 0) {
        char message[BUFFER_SIZE] = {0};
        int error_code = errno;
        sprintf(message, "last error code: %d\n", error_code);
        send(sock, message, strlen(message) + 1, 0);
    }
    else if (strcmp(command, "cursor") == 0) {
        char message[BUFFER_SIZE] = {0};
        int x = -1, y = -1;
        getyx(stdscr, y, x);
        sprintf(message, "cursor position: y = %d, x = %d\n", y, x);
        send(sock, message, strlen(message) + 1, 0);
    }
    else {
        const char *error_message = "invalid command\n";
        send(sock, error_message, strlen(error_message) + 1, 0);
    }
}

void *out(void *arg) {
    while (getchar() != 'q') { sleep(1); }
    printw("Shutdown server\n");
    refresh();
    endwin();
    exit(0);
}