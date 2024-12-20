#include "logger.h"

#define NAME1 "server_1.log"
#define NAME2 "server_2.log"
#define FILE1 "/tmp/server_1"
#define FILE2 "/tmp/server_2"

void *write_log(void *arg);
void *out(void *arg);

int main() {    
    // strcpy(file1, "/tmp/server_1");
    // int fd1 = mkfifo(PIPE_NAME, 0666);

    // fd1 = mkfifo(file1, 0666);
    // // fd1 = open(file1, O_RDONLY | O_TRUNC | O_CREAT, 777);
    // // perror("fd1 open 1");
    // // printf("here11\n");
    // if (fd1 == -1) {
    //     perror("mkfifo");
    //     fd1 = open(file1, O_RDONLY | O_TRUNC, 777);
    //     perror("fd1 open 1");
    //     if (fd1 == -1) {
    //         return 1;
    //     }
    //     // return 1;
    // }
    // strcpy(file2, "/tmp/server_2");
    // // fd1 = open(file1, O_RDONLY | O_TRUNC | O_CREAT, 777);
    // // perror("fd1 open 1");
    // // printf("here12\n");
    // fd2 = mkfifo(file2, 0666);
    // if (fd2 == -1) { 
    //     perror("mkfifo");
    //     fd2 = open(file2, O_RDONLY | O_TRUNC, 777);
    //     perror("fd1 open 2");
    //     if (fd2 == -1) {
    //         close(fd1);
    //         return 1;
    //     }
    //     // return 1; 
    // }
    

    // pthread_t check_cli;
    // if (pthread_create(&check_cli, NULL, out, NULL)) {
    //     close(fd1);
    //     return 1;
    // }
    // pthread_detach(check_cli);
    // printf("Start log server %s\n", argv[1]);
    // pthread_mutex_init(&mutex, NULL);
    pthread_t serv1;
    pthread_t serv2;
    char *name1 = NAME1;
    char *name2 = NAME2;
    if (pthread_create(&serv1, NULL, write_log, (void*)name1)) {
        // close(fd1);
        // close(fd2);
        return 1;
    }
    pthread_detach(serv1);
    if (pthread_create(&serv2, NULL, write_log, (void*)name2)) {
        // close(fd1);
        // close(fd2);
        return 1;
    }
    pthread_detach(serv2);
    printf("Start log server\n");
    // FILE *f = NULL;
    // ssize_t bytes = 0;
    // if ((f = fopen(name, "a")) != NULL) {
    //     while (1) {
    //         char buffer[BUFFER_SIZE] = {0};
    //         bytes = read(fd1, buffer, BUFFER_SIZE);
    //         if (strcmp(buffer, "-1") == 0) { break; }
    //         if (bytes > 0) {
    //                 // printf("Read %lu bytes\n", bytes);
    //                 printf("%s\n", buffer);
    //                 fputs(buffer, f);
    //         } else if (bytes == -1) {
    //             printf("Read error\n");
    //             break;
    //         } 
    //     }
    //     fclose(f);
    // }
    getchar();
    printf("End log server\n");
    // close(fd1);
    // close(fd2);
    return 0;
}

void *write_log(void *arg) {
    char *name = (char *)arg;
    char *file = (strcmp(name, NAME1) == 0) ? FILE1 : FILE2;
    int fd = mkfifo(file, 0666);
    if (fd == -1) {
        perror("mkfifo");
        fd = open(file, O_RDONLY | O_TRUNC, 777);
        perror("fd open");
        if (fd == -1) {
            return NULL;
        }
    }
    FILE *f = NULL;
    ssize_t bytes = 0;
    if ((f = fopen(name, "a")) != NULL) {
        while (1) {
            char buffer[BUFFER_SIZE] = {0};
            bytes = read(fd, buffer, BUFFER_SIZE);
            if (bytes > 0) {
                    // printf("Read %lu bytes\n", bytes);
                    printf("%s\n", buffer);
                    fputs(buffer, f);
            } else if (bytes == -1) {
                printf("Read error\n");
                break;
            } 
        }
        fclose(f);
    }
    close(fd);
}
