#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CONNECTION 3
#define MAX_BUFFER_SIZE 1024

void error(char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

int tcp_listen(int port) {
    int socket_fd;
    struct sockaddr_in addr;
    int return_code;
    int reuse_option = 1;   // 1: valid

    socket_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );
    if (socket_fd < 0) {
        error("Creating socket failed");
    }

    bzero((char *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    setsockopt(
        socket_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &reuse_option,
        sizeof(reuse_option)
    );
    
    return_code = bind(
        socket_fd,
        (struct sockaddr *) &addr,
        sizeof(addr)
    );
    if (return_code < 0) {
        close(socket_fd);
        error("Binding failed");
    }

    return_code = listen(
        socket_fd,
        MAX_CONNECTION
    );
    if (return_code < 0) {
        close(socket_fd);
        error("Listening failed");   
    }

    return socket_fd;
}

int main(int argc, char* argv[]) {
    int listen_fd;
    int connect_fd;
    struct sockaddr_storage client_addr;
    unsigned int client_addr_size = sizeof(client_addr);
    int return_code;
    char buf[MAX_BUFFER_SIZE];
    
    listen_fd = tcp_listen(11111);
    puts("Waiting for connection...");
    connect_fd = accept(
        listen_fd,
        (struct sockaddr *) &client_addr,
        &client_addr_size
    );
    if (connect_fd < 0) {
        close(listen_fd);
        error("Connecting a client failed");
    }

    return_code = recv(connect_fd, buf, MAX_BUFFER_SIZE, 0);
    if (return_code < 0) {
        close(connect_fd);
        close(listen_fd);
        error("Recv failed");
    }

    write(1, buf, return_code);
    return_code = send(connect_fd, buf, return_code, 0);

    close(connect_fd);
    close(listen_fd);

    return 0;
}