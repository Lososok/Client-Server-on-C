#include "server_1.h"

void init_ncerses();
int init_port(int *server_fd, struct sockaddr_in *server_addr);
int realloc_pthreads_arr(server_pthreads *ser_pth);
int start_session(server_pthreads *ser_pth, int cl_fd);
void *session(void *arg);

// int main(int argc, const char **argv) {
//     // if (argc < 2) { 
//     //     fprintf(stderr, "Write port\n"); 
//     //     return 1; 
//     // }
//     // int port = atoi(argv[1]);
//     // if (!port) {
//     //     fprintf(stderr, "Uncorrected port\n"); 
//     //     return 1;
//     // }
//     int server_fd = 0, current_cl = 0;;
//     clients_fd cl_fd = {2, (int*)malloc(2 * sizeof(int))};
//     server_pthreads cl_pth = {2, (pthread_t*)malloc(2 * sizeof(pthread_t))};
//     struct sockaddr_in server_addr;
//     char buffer[MAX_BUFFER_SIZE] = {0};
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { perror("socket"); exit(1); }
//     // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { exit(1); }
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);
//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { perror("bind"); exit(1); }
//     if (listen(server_fd, SOMAXCONN) < 0) { printf("here3\n"); exit(1); }

//     while (1) {
//         if ((cl_fd.clients[current_cl] = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t*)&server_addr)) < 0) { continue; }
        
//         read(cl_fd.clients[current_cl], buffer, MAX_BUFFER_SIZE);
//         printf("Received message: %s\n", buffer);

//         send(cl_fd.clients[current_cl], "EAP", strlen("EAP"), 0);

//         close(cl_fd.clients[current_cl]);
//     }
//     free(cl_fd.clients);
//     free(cl_pth.pthreads);
//     return 0;
// }


int main() {
    int server_fd = 0;
    // clients_fd cl_fd = {2, (int*)malloc(2 * sizeof(int))};
    server_pthreads ser_pth = {2, (int*)calloc(2, sizeof(int)), (pthread_t*)malloc(2 * sizeof(pthread_t))};
    struct sockaddr_in server_addr;
    // char buffer[MAX_BUFFER_SIZE] = {0};
    // init_ncerses();
    if (!init_port(&server_fd, &server_addr)) { 
        if (ser_pth.pthreads) { free(ser_pth.pthreads); }
        return 1;
    }
    
    while (1) {
        // if (getch() == 'q') {
        //     if (ser_pth.pthreads) { free(ser_pth.pthreads); }
        //     endwin();
        //     return 0;
        // }
        int cl_fd = -1;
        if ((cl_fd = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t*)&server_addr)) != -1) {
            if (!start_session(&ser_pth, cl_fd)) {
                perror("Connet error");
            } else {
                printf("Get conect with client %d\n", cl_fd);
            }
        }
    }
    for (int i = 0; i < ser_pth.count_pthreads; i++) {
        pthread_join(ser_pth.pthreads[i], NULL);
    }
    return 0;
}


void init_ncerses() {
    initscr();
    cbreak();
    noecho();
}

int init_port(int *server_fd, struct sockaddr_in *server_addr) {
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { perror("socket"); return 0; }
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { exit(1); }
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);
    if (bind(*server_fd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) { perror("bind"); return 0; }
    if (listen(*server_fd, SOMAXCONN) < 0) { printf("here3\n"); return 0; }
    return 1;
}

int realloc_pthreads_arr(server_pthreads *ser_pth) {
    pthread_t *temp  = realloc(ser_pth->pthreads, ser_pth->count_pthreads * 2 * sizeof(pthread_t));
    if (temp) {
        ser_pth->pthreads[ser_pth->count_pthreads - 1];
        ser_pth->count_pthreads *= 2;
        ser_pth->pthreads = temp;
    } else { return 0; }
    return 1;
}

int start_session(server_pthreads *ser_pth, int cl_fd) {
    int rez = 1, i = 0;
    char buffer[20] = {0};
    for (; ser_pth->list_used_pthreads[i] != 0 && i < ser_pth->count_pthreads; i++) {
    }
    if (i == ser_pth->count_pthreads - 1) { realloc_pthreads_arr(ser_pth); }
    pthread_t temp;
    sprintf(buffer, "%d", cl_fd);
    if (pthread_create(&temp, NULL, session, (void*)(buffer))) {
        perror("Thread create error");
        rez = 0;
    }
    ser_pth->pthreads[i] = temp;
    ser_pth->list_used_pthreads[i] = 1;

    // if (read(cl_fd, buffer, MAX_BUFFER_SIZE) == -1) { rez = 0; }
    // else { printf("Message from client with fd = %d: %s\n", cl_fd, buffer);}
    // if (send(cl_fd, "EAP", strlen("EAP"), 0)) { rez = 0; }
    // close(cl_fd);
    return rez;
}

void *session(void *arg) {
    int cl_fd = atoi(arg);
    char buffer[MAX_BUFFER_SIZE] = {0};
    if (read(cl_fd, buffer, MAX_BUFFER_SIZE) == -1) {  }
    else {
        printf("Message from client with fd = %d: %s\n", cl_fd, buffer);
        printw("Message from client with fd = %d: %s\n", cl_fd, buffer);
        // refresh();
    }
    if (send(cl_fd, "EAP", strlen("EAP"), 0)) {  }
    close(cl_fd);
    return NULL;
}
