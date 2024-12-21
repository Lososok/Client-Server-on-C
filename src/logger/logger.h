#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define NAME1 "server_1.log"
#define NAME2 "server_2.log"
#define FILE1 "/tmp/server_1"
#define FILE2 "/tmp/server_2"

void *write_log(void *arg);
void *out(void *arg);

#endif