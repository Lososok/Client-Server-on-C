#include "server_1.h"

int init_port(int *server_fd, struct sockaddr_in *server_addr, int *enable);
void init_ncerses();
int start_session(int cl_fd);
void *session(void *arg);
void *out(void *arg);
int send_error_code();
int send_cursor_position();

pthread_mutex_t term;

int main() {
    init_ncerses();
    printw("Run server\n");
    printw("Press q to start shutdown\n");
    pthread_mutex_init(&term, NULL);
    refresh();
    int server_fd = 0, enable = 1;
    struct sockaddr_in server_addr;
    int addrlen = sizeof(server_addr);
    if (!init_port(&server_fd, &server_addr, &enable)) { 
        endwin();
        pthread_mutex_destroy(&term);
        return 1;
    }
    
    pthread_t check_cli;
    if (pthread_create(&check_cli, NULL, out, NULL)) {
        perror("Thread CLI create error");
        endwin();
        pthread_mutex_destroy(&term);
        return 1;
    }
    pthread_detach(check_cli);
    while (1) {
        int cl_fd = -1;
        if ((cl_fd = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen) != -1)) {
            if (!start_session(cl_fd)) {
                perror("Connet error");
            } else {
                pthread_mutex_lock(&term);
                printw("Get conect with client %d\n", cl_fd);
                refresh();
                pthread_mutex_unlock(&term);
            }
        } else {
            pthread_mutex_lock(&term);
            printw("Connet error");
            refresh();
            pthread_mutex_unlock(&term);
        }
    }
    pthread_mutex_destroy(&term);
    endwin();
    return 0;
}

void init_ncerses() {
    initscr();
    cbreak();
    noecho();
}

int init_port(int *server_fd, struct sockaddr_in *server_addr, int *enable) {
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { perror("socket"); return 0; }
    // if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, enable, sizeof(int)) != -1) { perror("setsockopt error"); return 0; }
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);                    // DIFF
    if (bind(*server_fd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) { perror("bind"); return 0; }
    if (listen(*server_fd, SOMAXCONN) < 0) { printf("here3\n"); return 0; }
    return 1;
}

int start_session(int cl_fd) {
    int rez = 1;
    int *new_sock = malloc(1);
    *new_sock = cl_fd;
    // char buffer[20] = {0};
    pthread_t temp;
    // sprintf(buffer, "%d", *new_sock);
    if (pthread_create(&temp, NULL, session /*DIFF*/, (void*)(new_sock))) {
        perror("Thread create error");
        rez = 0;
        free(new_sock);
    }
    pthread_detach(temp);
    return rez;
}

void *session(void *arg) {
    int cl_fd = *(int*)arg;
    char buffer[MAX_BUFFER_SIZE] = {0};
    pthread_mutex_lock(&term);
    // if (recv(cl_fd, buffer, MAX_BUFFER_SIZE, 0) == -1) { printw("here1\n"); refresh(); }
    if (read(cl_fd, buffer, MAX_BUFFER_SIZE) == -1) { printw("here1\n"); refresh(); }
    else {
        printw("here2\n");
        refresh();
        if (strcmp(buffer, "errorc") == 0) {
            if (send_error_code()) {
                printw("Send last error code to %d\n", cl_fd);
                refresh();
            } else { perror("send_error_code error"); }
            printw("here3\n");
            refresh();
        }
        else if (strcmp(buffer, "cursor") == 0) {
            if (send_cursor_position()) {
                printw("Send curor position to %d\n", cl_fd);
                refresh();
            } else { perror("send_cursor_position error"); }
            printf("here4\n");
        }
        printw("Message from client with fd = %d: %s\n", cl_fd, buffer);
        refresh();
    }
    if (send(cl_fd, "EAP", strlen("EAP"), 0)) { printw("here5\n"); refresh(); }
    printw("Close connect with %d\n", cl_fd);
    refresh();
    close(cl_fd);
    free(arg);
    pthread_mutex_unlock(&term);
    return NULL;
}

void *out(void *arg) {
    while (getchar() != 'q') {
        sleep(1);
    }
    printw("Shutdown server\n");
    printw("Press q to exit\n");
    refresh();
    getchar();
    pthread_mutex_destroy(&term);
    endwin();
    system("exec bash");
    exit(0);
}

int send_error_code() {
    return 0;
}

int send_cursor_position() {
    return 0;
}