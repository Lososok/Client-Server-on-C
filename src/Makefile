.PHONY: all client server1 server2

CC=gcc
LIBS=-lncurses

all: server1 server2 client

server1:
	$(CC) server_1.c $(LIBS) -o server1

server2:
	$(CC) server_2.c $(LIBS) -o server2

client:
	$(CC) client.c $(LIBS) -o client
