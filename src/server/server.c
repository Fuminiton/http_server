#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server.h"
#include "utils.h"

int create_listener_socket(void) {
    int listener_descriptor;
    struct sockaddr_in addr;
    int reuse_option;
    int return_code;

    // socket作成
    listener_descriptor = socket(PF_INET, SOCK_STREAM, 0);
    if (listener_descriptor < 0) {
        error("creating a listener socket failed: %s", strerror(errno));
    }

    // アドレス設定
    bzero(&addr, sizeof(addr)); 
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // SO_REUSEADDR設定
    reuse_option = 1;
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

    // bind
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
    return_code = listen(listener_descriptor, PORT);
    if (return_code < 0) {
        close(listener_descriptor);
        error("listen failed: %s", strerror(errno));
    }

    return listener_descriptor;
}

int accept_connection(int listener_descriptor) {
    struct sockaddr_in addr;
    socklen_t address_size;
    int connect_descriptor;

    address_size = sizeof(addr);
    connect_descriptor = accept(
        listener_descriptor,
        (struct sockaddr *)&addr,
        &address_size
    );
    if (connect_descriptor < 0) {
        close(listener_descriptor);
        error("creating a connect socket failed: %s", strerror(errno));
    }

    return connect_descriptor;
}

void handle_client(int connect_descriptor) {
    char buf[BUFFER_SIZE];
    int return_code;

    // クライアントからデータ読み込み
    return_code = read(connect_descriptor, buf, BUFFER_SIZE);
    if (return_code < 0) {
        close(connect_descriptor);
        error("read failed: %s", strerror(errno));
    }

    // 標準出力に書き込み
    return_code = write(1, buf, return_code);
    if (return_code < 0) {
        close(connect_descriptor);
        error("write failed: %s", strerror(errno));
    }
}