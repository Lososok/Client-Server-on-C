#include "server.h"

#define PORT 8080

int init_port(int *server_fd, int *opt, struct sockaddr_in *address);
void *session(void *arg);
void command_handler(char *command, int sock);
void *out(void *arg);
char *show_time(char *src);
void create_log_note(char *buf);

const char *pipe_name = "/tmp/server_1";
int fd;

int main() {
    system("./logger.exe 1");
    fd = open(pipe_name, O_WRONLY);
    if (fd == -1) { perror("open"); }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if (!init_port(&server_fd, &new_socket, &address)) { 
        close(fd);
        return 1; 
    }

    initscr();
    printw("Run server 1\n");
    create_log_note("Run server 1\n");
    refresh();

    pthread_t check_cli;
    if (pthread_create(&check_cli, NULL, out, NULL)) {
        perror("Thread CLI create error");
        create_log_note("Thread CLI create error");
        endwin();
        close(fd);
        return 1;
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept error");
            create_log_note("accept error");
            endwin();
            close(fd);
            return 1;
        }

        printw("Connect client %d on server 1\n", new_socket);
        create_log_note("Connect client");
        refresh();

        pthread_t thread_id;
        int *new_sock = malloc(1);
        *new_sock = new_socket;

        if (pthread_create(&thread_id, NULL, session, (void *)new_sock) < 0) {
            perror("pthread create error");
            create_log_note("pthread create error");
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
        create_log_note("socket failed");
        return 0;
    }

    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, opt, sizeof(*opt))) {
        perror("setsockopt error");
        create_log_note("setsockopt error");
        return 0;
    }
    
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("bind failed");
        create_log_note("bind failed");
        return 0;
    }

    if (listen(*server_fd, SOMAXCONN) < 0) {
        perror("listen error");
        create_log_note("listen error");
        return 0;
    }
    return 1;
}

void *session(void *arg) {
    int sock = *(int *)arg;
    int read_size = 1;

    send(sock, "connect server 1\n", strlen("connect server 1\n") + 1, 0);
    char create_log_note_message[128] = {0};
    sprintf(create_log_note_message, "send confirmation to client %d", sock);
    create_log_note(create_log_note_message);

    while (read_size > 0) {
        char command_buffer[BUFFER_SIZE] = {0};
        read_size = read(sock, command_buffer, BUFFER_SIZE);
        command_handler(command_buffer, sock);
    }

    if (read_size == 0) {
        printw("client %d disconnect\n", sock);
        char create_log_note_message[128] = {0};
        sprintf(create_log_note_message, "client %d disconnect", sock);
        create_log_note(create_log_note_message);
        refresh();
    } else if (read_size == -1) {
        perror("read error");
        create_log_note("read error");
    }

    free(arg);
    close(sock);
    return NULL;
}

void command_handler(char *command, int sock) {
    if (strcmp(command, "error") == 0) {
        char message[BUFFER_SIZE] = {0};
        char current_time[128] = {0};
        int error_code = errno;
        show_time(current_time);
        sprintf(message, "%s: last error code: %d\n", current_time, error_code);
        send(sock, message, strlen(message) + 1, 0);
        char create_log_note_message[128] = {0};
        sprintf(create_log_note_message, "send client %d last error", sock);
        create_log_note(create_log_note_message);
    }
    else if (strcmp(command, "cursor") == 0) {
        char message[BUFFER_SIZE] = {0};
        char current_time[128] = {0};
        int x = -1, y = -1;
        getyx(stdscr, y, x);
        show_time(current_time);
        sprintf(message, "%s: cursor position: y = %d, x = %d\n", current_time, y, x);
        send(sock, message, strlen(message) + 1, 0);
        send(sock, message, strlen(message) + 1, 0);
        char create_log_note_message[128] = {0};
        sprintf(create_log_note_message, "send client %d cursor position", sock);
    }
    else {
        char message[BUFFER_SIZE] = {0};
        char current_time[128] = {0};
        show_time(current_time);
        sprintf(message, "%s: invalid command\n", current_time);
        send(sock, message, strlen(message) + 1, 0);
        send(sock, message, strlen(message) + 1, 0);
        char create_log_note_message[128] = {0};
        sprintf(create_log_note_message, "send client %d bad request", sock);
    }
}

void *out(void *arg) {
    while (getchar() != 'q') { sleep(1); }
    printw("Shutdown server\n");
    refresh();
    create_log_note("shutdown server");
    endwin();
    close(fd);
    exit(0);
}

char *show_time(char *src) {
    time_t current_time = time(NULL);
    struct tm *now = localtime(&current_time);
    sprintf(src, "TIME:\t%.2d:%.2d:%.2d", now->tm_hour, now->tm_min, now->tm_sec);
}

void create_log_note(char *buf) {
    char data[BUFFER_SIZE] = {0};
    time_t current_datetime = time(NULL);
    struct tm *now = localtime(&current_datetime);
    sprintf(data, "DATETIME:\t%.2d:%.2d:%.2d %.2d:%.2d:%.4d:\t%s", now->tm_hour, now->tm_min, now->tm_sec, now->tm_mday, now->tm_mon+1, now->tm_year+1900, buf);
    write(fd, data, strlen(data) + 1);
}