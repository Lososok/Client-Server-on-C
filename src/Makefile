.PHONY: all client server1 server2 logger run build_image rm_containers rm_executables clean exe

CC=gcc
LIBS=-lncurses -lc

all: exe build_image

server1:
	$(CC) servers/server_1.c $(LIBS) -o server1.exe

server2:
	$(CC) servers/server_2.c $(LIBS) -o server2.exe

logger:
	$(CC) logger/logger.c -o logger.exe

client:
	$(CC) client/client.c -o client.exe

exe: server1 server2 client logger

run:
	docker run -it --network host docker_client

build_image:
	sudo docker build -t docker_client .

rm_containers:
	docker rm -f $$(docker ps -aq)

rm_executables:
	rm -f *.exe

clean: rm_executables rm_containers