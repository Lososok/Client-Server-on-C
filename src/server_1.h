#ifndef SERVER_1_H
#define SERVER_1_H

#define PORT 1111
#define MAX_BUFFER_SIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
    int count_clients;
    int *clients;
} clients_fd;
typedef struct {
    int count_pthreads;
    int *list_used_pthreads;
    pthread_t *pthreads;
} server_pthreads;

#endif