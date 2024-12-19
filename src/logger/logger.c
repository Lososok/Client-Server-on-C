#include "logger.h"

void *write_log(void *arg);

int fd;
char file[256];
// pthread_mutex_t mutex;

int main(int argc, const char **argv) {
    // printf("here1\n");
    char name[128] = {0};
    if (argc != 2) { return 1; }
    if (argv[1][0] == '1') {
        strcpy(file, "temp/server_1");
        strcpy(name, "server_1.log");
        fd = open(file, O_RDONLY | O_TRUNC | O_CREAT, 777);
        perror("fd open 1");
        // printf("here11\n");
        if (fd == -1) { return 1; }
    } else if (argv[1][0] == '2') {
        strcpy(file, "temp/server_2");
        strcpy(name, "server_2.log");
        fd = open(file, O_RDONLY | O_CREAT);
        perror("fd open 1");
        // printf("here12\n");
        if (fd == -1) { return 1; }
    }
    printf("Start log server %s\n", argv[1]);
    // pthread_mutex_init(&mutex, NULL);
    FILE *f = NULL;
    ssize_t bytes = 0;
    if ((f = fopen(name, "a")) != NULL) {
        while (1) {
            char buffer[BUFFER_SIZE] = {0};
            bytes = read(fd, buffer, BUFFER_SIZE);
            // char *log_message = malloc((strlen(buffer) + 1) * sizeof(char));
            // strcpy(log_message, buffer);
            // pthread_t log_pthread;
            // if (strcmp(buffer, "-1") == 0) { break; }
            if (bytes > 0) {
                    printf("Read %lu bytes\n", bytes);
                    printf("%s\n", buffer);
                    fputs(buffer, f);
                    // fputc('\n', f);
                // free(log_message);
                // if (pthread_create(&log_pthread, NULL, write_log, (void *)log_message) < 0) {
                //     free(log_message);
                // }
                // pthread_detach(log_pthread);
            } else if (bytes == -1) {
                printf("Read error\n");
                break;
            } 
        }
        fclose(f);
    }
    printf("End log server %s\n", argv[1]);
    // printf("here1\n");
    close(fd);
    // pthread_mutex_destroy(&mutex);
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
        // pthread_mutex_lock(&mutex);
        fputs(log_message, f);
        fputc('\n', f);
        // pthread_mutex_unlock(&mutex);
        fclose(f);
    }
    free(arg);
}