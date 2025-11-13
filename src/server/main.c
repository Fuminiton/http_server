#include <unistd.h>
#include "server.h"

int main(void) {
    int listener_descriptor;
    int connect_descriptor;

    // リスナーソケット作成
    listener_descriptor = create_listener_socket();

    // 接続待ち受け
    connect_descriptor = accept_connection(listener_descriptor);

    // クライアント処理
    handle_client(connect_descriptor);

    // クリーンアップ
    close(connect_descriptor);
    close(listener_descriptor);

    return 0;
}