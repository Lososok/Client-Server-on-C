#include "server.h"

#define PORT 8081

int init_port(int *server_fd, int *opt, struct sockaddr_in *address);
void *session(void *arg);
void command_handler(char *command, int sock);
void *out(void *arg);
float virtual_and_pthysical_memory_used();
char *show_time(char *src);
void create_log_note(char *buf);

const char *pipe_name = "/tmp/server_2";
int fd;

int main() {
    FILE *pipe = popen("ps -a | grep logger.exe", "r");
    if (pipe != NULL) {
        char buffer[BUFFER_SIZE] = {0};
        fgets(buffer, BUFFER_SIZE, pipe);
        if (strlen(buffer) == 0) {
            printf("Logger dosent exit\n");
            return 1;
        }
        pclose(pipe);
    } else {
        perror("fopen");
        return 1;
    }

    fd = open(pipe_name, O_WRONLY, 066);
    if (fd == -1) { perror("open pipe error"); return 1; }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    initscr();
    if (!init_port(&server_fd, &new_socket, &address)) { 
        close(fd);
        endwin();
        return 1;
    }

    printw("Run server 2\n");
    create_log_note("Run server 2");
    refresh();

    pthread_t check_cli;
    if (pthread_create(&check_cli, NULL, out, NULL)) {
        perror("Thread CLI create error");
        create_log_note("Thread CLI create error");
        close(fd);
        endwin();
        return 1;
    }
    pthread_detach(check_cli);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept error");
            create_log_note("Accept error");
            close(fd);
            endwin();
            return 1;
        }

        printw("Connect client %d on server 2\n", new_socket);
        char create_log_note_message[128] = {0};
        sprintf(create_log_note_message, "Connect client %d on server 2", new_socket);
        create_log_note(create_log_note_message);
        refresh();

        pthread_t thread_id;
        int *new_sock = malloc(1);
        *new_sock = new_socket;

        if (pthread_create(&thread_id, NULL, session, (void *)new_sock) < 0) {
            perror("pthread create error");
            create_log_note("Pthread create error");
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
        create_log_note("Socket failed");
        return 0;
    }

    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, opt, sizeof(*opt))) {
        perror("setsockopt");
        create_log_note("Setsockopt error");
        return 0;
    }
    
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("bind failed");
        create_log_note("Bind failed");
        return 0;
    }

    if (listen(*server_fd, SOMAXCONN) < 0) {
        perror("listen");
        create_log_note("Listen error");
        return 0;
    }
    return 1;
}

void *session(void *arg) {
    int sock = *(int *)arg;
    int read_size = 1;

    send(sock, "connect server 2\n", strlen("connect server 2\n") + 1, 0);
    char create_log_note_message[128] = {0};
    sprintf(create_log_note_message, "Send confirmation to client %d", sock);
    create_log_note(create_log_note_message);

    while (read_size > 0) {
        char command_buffer[BUFFER_SIZE] = {0};
        read_size = read(sock, command_buffer, BUFFER_SIZE);
        command_handler(command_buffer, sock);
    }

    if (read_size == 0) {
        printw("client %d disconnect\n", sock);
        refresh();
        char create_log_note_message[128] = {0};
        sprintf(create_log_note_message, "Client %d disconnect", sock);
        create_log_note(create_log_note_message);
    } else if (read_size == -1) {
        perror("read error");
        create_log_note("Read error");
    }

    free(arg);
    close(sock);
    return NULL;
}

void command_handler(char *command, int sock) {
    if (strcmp(command, "physical") == 0) {
        char message[BUFFER_SIZE] = {0};
        char current_time[128] = {0};
        show_time(current_time);
        float rez = virtual_and_pthysical_memory_used();
        sprintf(message, "%s: percentage of pthysical memory used: %.2f%%\n", current_time, rez);
        send(sock, message, strlen(message) + 1, 0);

        char create_log_note_message[128] = {0};
        sprintf(create_log_note_message, "Send client %d percentage of pthysical memory used", sock);
        create_log_note(create_log_note_message);

        printw("Send client %d percentage of pthysical memory used\n", sock);
        refresh();
    }
    else if (strcmp(command, "virtual") == 0) {
        char message[BUFFER_SIZE] = {0};
        char current_time[128] = {0};
        show_time(current_time);
        float rez = virtual_and_pthysical_memory_used();
        sprintf(message, "%s: percentage of virtual memory used: %.2f%%\n", current_time, rez);
        send(sock, message, strlen(message) + 1, 0);

        char create_log_note_message[128] = {0};
        sprintf(create_log_note_message, "Send client %d percentage of virtual memory used", sock);
        create_log_note(create_log_note_message);

        printw("Send client %d percentage of virtual memory used\n", sock);
        refresh();
    }
}

void *out(void *arg) {
    while (getchar() != 'q') { sleep(1); }
    printw("Shutdown server 1\n");
    refresh();
    create_log_note("Shutdown server 2");
    endwin();
    close(fd);
    exit(0);
}

float virtual_and_pthysical_memory_used() {
    struct sysinfo info;

    if (sysinfo(&info) != 0) {
        perror("sysinfo");
        create_log_note("Sysinfo error");
        exit(1);
    }

    long totalMemory = info.totalram;
    long freeMemory = info.freeram;
    long usedMemory = totalMemory - freeMemory;

    return ((float)usedMemory / totalMemory) * 100;
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
    sprintf(data, "DATETIME:\t%.2d:%.2d:%.2d %.2d:%.2d:%.4d:\t%s\n", now->tm_hour, now->tm_min, now->tm_sec, now->tm_mday, now->tm_mon+1, now->tm_year+1900, buf);
    ssize_t bytes = write(fd, data, strlen(data) + 1);
}