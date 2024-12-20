FROM gcc:latest

COPY ./client/client.c /DockerClient/client.c

WORKDIR /DockerClient

RUN gcc client.c -o client.exe -lpthread

ENTRYPOINT ["./client.exe"]
