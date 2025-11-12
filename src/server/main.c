#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>


#define PORT 30000
#define BUFFER_SIZE 12

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);   
}


int main() {
    int listener_descriptor;
    int connect_descriptor;
    struct sockaddr_in addr;
    int address_size;
    int reuse_option;
    int return_code;
    char buf[BUFFER_SIZE];

    // bind
    listener_descriptor = socket(
        PF_INET,
        SOCK_STREAM,
        0
    );
    if (listener_descriptor < 0) {
        error("creating a listener socket failed: %s", strerror(errno));
    }

    bzero(&addr, sizeof(addr)); 
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    reuse_option = 1;  // 1: valid
    return_code = setsockopt(
        listener_descriptor,
        SOL_SOCKET,
        SO_REUSEADDR,
        &reuse_option,
        sizeof(reuse_option)
    );
    if (return_code < 0) {
        close(listener_descriptor);
        error("socket option setting failed: %s", strerror(errno));
    }

    return_code = bind(
        listener_descriptor,
        (struct sockaddr *) &addr,
        sizeof(addr)
    );
    if (return_code < 0) {
        close(listener_descriptor);
        error("bind failed: %s", strerror(errno));
    }

    // listen
    return_code = listen(
        listener_descriptor,
        PORT
    );
    if (return_code < 0) {
        close(listener_descriptor);
        error("listen failed: %s", strerror(errno));
    }

    // accept
    address_size = sizeof(addr);
    connect_descriptor = accept(
        listener_descriptor,
        &addr,
        (socklen_t*) &address_size
    );
    if (connect_descriptor < 0) {
        close(listener_descriptor);
        error("creating a connect socket failed: %s", strerror(errno));
    }

    // begin
    return_code = read(
        connect_descriptor,
        buf,
        BUFFER_SIZE
    );
    if (return_code < 0) {
        close(connect_descriptor);
        close(listener_descriptor);
        error("read failed: %s", strerror(errno));
    }

    return_code = write(
        1,
        buf,
        return_code
    );
    if (return_code < 0) {
        close(connect_descriptor);
        close(listener_descriptor);
        error("write failed: %s", strerror(errno));
    }
    close(connect_descriptor);
    close(listener_descriptor);
    return 0;
}
