# Client-Server-on-C
#### A client-server project with 2 servers receiving requests from any number of clients and sending back A response. There are also third server that logs the servers

#### Description
1. The main servers communicate with clients using sockets
2. The log server connect with the main servers by named pipe
3. Client can connect/disconnect to the first or/and second server add request data
4. The first server can show the last error code in it and the current carriage position
5. The secon server can show the percentage of physical and virtual memory used on it (actualy show indetical values, idk how to count correctly)
6. The log server write logs of all events in the main servers
## Build all
    make
## Build .exe files
    make exe
## Run docker
### Buid image
    make image
### Run container 
    make run
