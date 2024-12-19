.PHONY: all client server1 server2 logger

CC=gcc
LIBS=-lncurses

all: server1 server2 client logger

server1:
	$(CC) servers/server_1.c $(LIBS) -o server1.exe

server2:
	$(CC) servers/server_2.c $(LIBS) -o server2.exe

logger:
	$(CC) logger/logger.c -o logger.exe

client:
	$(CC) client/client.c -o client.exe
