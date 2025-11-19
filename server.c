#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 11111
#define MAX_CONNECTION 3
#define MAX_BUFFER_SIZE 1024


int tcp_listen(int port) {
    int socket_fd;
    struct sockaddr_in addr;
    int return_code;
    int opt = 1;   // 1: valid

    // create socket
    socket_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );
    if (socket_fd < 0) {
        fprintf(stderr, "Creating socket failed.\n");
        exit(1);
    }

    // set address
    bzero((char *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    // set socket options
    return_code = setsockopt(
        socket_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );
    if (return_code < 0) {
        close(socket_fd);
        fprintf(stderr, "Setting socket options failed.\n");
        exit(1);
    }

    // bind
    return_code = bind(
        socket_fd,
        (struct sockaddr *) &addr,
        sizeof(addr)
    );
    if (return_code < 0) {
        close(socket_fd);
        fprintf(stderr, "Binding failed.\n");
        exit(1);
    }

    // listen
    return_code = listen(
        socket_fd,
        MAX_CONNECTION
    );
    if (return_code < 0) {
        close(socket_fd);
        fprintf(stderr, "Listening failed.\n");
        exit(1);   
    }

    return socket_fd;
}

int connect_client(int listen_fd) {
    int connect_fd;
    struct sockaddr_storage client_addr;
    unsigned int client_addr_size = sizeof(client_addr);

    connect_fd = accept(
        listen_fd,
        (struct sockaddr *) &client_addr,
        &client_addr_size
    );
    if (connect_fd < 0) {
        fprintf(stderr, "Connecting a client failed\n");
    }

    return connect_fd;
}

void handle_client(int connect_fd) {
    char* buf[MAX_BUFFER_SIZE];
    int return_code = recv(connect_fd, buf, MAX_BUFFER_SIZE, 0);
    if (return_code < 0) {
        close(connect_fd);
    }
    write(1, buf, return_code);
    return_code = send(connect_fd, buf, return_code, 0);
    close(connect_fd);
}

int main(int argc, char* argv[]) {
    int listen_fd;
    int connect_fd;
   
    listen_fd = tcp_listen(PORT);
    printf("Server is running on port %d...\n", PORT);

    while (1) {
        connect_fd = connect_client(listen_fd);
        if (connect_fd < 0) {
            continue;
        }
        handle_client(connect_fd);
    }
    close(listen_fd);

    return 0;
}