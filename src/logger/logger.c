#include "logger.h"

void *write_log(void *arg);

int fd;
char file[256];
pthread_mutex_t mutex;

int main(int argc, const char **argv) {
    if (argc < 2) { return 1; }
    if (argv[1][0] == '1') {
        strcpy(file, "/tmp/server_1");
        fd = open(file, O_RDONLY);
        if (fd == -1) { return 1; }
    } else if (argv[1][0] == '2') {
        strcpy(file, "/tmp/server_2");
        fd = open(file, O_RDONLY);
        if (fd == -1) { return 1; }
    }
    pthread_mutex_init(&mutex, NULL);
    ssize_t bytes = 1;
    while (bytes) {
        char buffer[BUFFER_SIZE] = {0};
        bytes = read(fd, buffer, BUFFER_SIZE);
        char *log_message = malloc((strlen(buffer) + 1) * sizeof(char));
        strcpy(log_message, buffer);
        pthread_t log_pthread;

        if (pthread_create(&log_pthread, NULL, write_log, (void *)log_message) < 0) {
            free(log_message);
        }
        pthread_detach(log_pthread);
    }
    close(fd);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void *write_log(void *arg) {
    char *log_message = (char *)arg;
    char name[128];
    FILE *f = NULL;
    if (file[strlen(file) - 1] == '1') {
        strcpy(name, "server_1.log");
    } else if (file[strlen(file) - 1] == '2') {
        strcpy(name, "server_2.log");
    }
    if ((f = fopen(name, "a")) != NULL) {
        pthread_mutex_lock(&mutex);
        fputs(log_message, f);
        pthread_mutex_unlock(&mutex);
        fclose(f);
    }
    free(arg);
}