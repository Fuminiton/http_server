#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define MAX_CONNECTION 3
#define MAX_BUFFER_SIZE 1024

void error(char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

int tcp_connect(const char *hostname, int port) {
    int socket_fd;
    struct sockaddr_in addr;
    int return_code;
    struct hostent *host;

    socket_fd = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );
    if (socket_fd < 0) {
        error("Creating socket failed");
    }

    bzero((char *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    host = gethostbyname(hostname);
    addr.sin_addr = *(struct in_addr *) (host->h_addr_list[0]);
    addr.sin_port = htons(port);
    
    return_code = connect(
        socket_fd,
        (struct sockaddr *) &addr,
        sizeof(addr)
    );
    if (return_code < 0) {
        close(socket_fd);
        error("Connecting failed");
    }

    return socket_fd;
}

int main(int argc, char* argv[]) {
    int socket_fd;
    char *input;
    char buf[MAX_BUFFER_SIZE];
    char* hostname;
    int port;
    int return_code;
    
    if (argc != 3) {
        printf("usage: %s [ip address] [port]\n", argv[0]);
    }

    hostname = argv[1];
    port = atoi(argv[2]);

    socket_fd = tcp_connect(hostname, port);
    
    input = fgets(buf, MAX_BUFFER_SIZE, stdin);
    return_code = send(socket_fd, input, strlen(input), 0);
    if (return_code < 0) {
        close(socket_fd);
        error("Send failed");
    }

    return_code = recv(socket_fd, buf, sizeof(buf), 0);
    if (return_code < 0) {
        close(socket_fd);
        error("Recv failed");
    }

    write(1, buf, return_code);

    close(socket_fd);
    
    return 0;
}
