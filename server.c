#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CONNECTIONS 10
#define MAX_RESPONSE_SIZE 8192
#define MAX_RESULT_STR 32
#define BUFFER_SIZE 4096

#define HTTP_OK 200
#define HTTP_BAD_REQUEST 400
#define HTTP_NOT_FOUND 404
#define HTTP_INTERNAL_ERROR 500


/* =======================================================================
 * データ構造
 * ==================================================================== */

typedef struct {
    char *method;
    char *path;
    char *protocol;
} HTTPRequest;


/* =======================================================================
 * ユーティリティ関数
 * ==================================================================== */

void log_exit(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "FATAL: ");
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void log_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
}

void* xmalloc(size_t size) {
    void *pointer = malloc(size);
    if (!pointer) {
        log_exit("Memory allocation failed for %zu bytes", size);
    }
    memset(pointer, 0, size);
    return pointer;
}

char* xstrdup(const char *s) {
    if (!s) {
        return NULL;
    }
    char *copy = strdup(s);
    if (!copy) {
        log_exit("String duplication failed");
    }
    return copy;
}


/* =======================================================================
 * HTTPリクエスト処理
 * ==================================================================== */

HTTPRequest* create_request() {
    HTTPRequest *request = xmalloc(sizeof(HTTPRequest));
    return request;
}

int parse_request(HTTPRequest *request, const char *buffer) {
    if (!request || !buffer) {
        return -1;
    }

    char method[16];
    char path[256];
    char protocol[16];

    int matched = sscanf(buffer, "%15s %255s %15s", method, path, protocol);

    request->method = xstrdup(method);
    request->path = xstrdup(path);
    request->protocol = xstrdup(protocol);

    if (!request->method || !request->path || !request->protocol) {
        log_error("Invalid HTTP request format");
        return -1;
    }

    return 0;
}

void free_request(HTTPRequest *request) {
    if (!request) {
        return ;
    }
    free(request->method);
    free(request->path);
    free(request->protocol);
    free(request);
}


/* ============================================================================
 * HTTPレスポンス生成
 * ========================================================================== */

void build_response(char *buffer, size_t buffer_size,
                    int status_code, const char *status_text,
                    int content_length, const char *content) { 
    snprintf(buffer, buffer_size,
            "HTTP/1.1 %d %s\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s\n",
            status_code, status_text,
            content_length, content
        );
}

void build_success_response(char *buffer, size_t buffer_size, int result) {
    char content[MAX_RESULT_STR];

    snprintf(content, sizeof(content), "%d", result);
    build_response(buffer, buffer_size, HTTP_OK, "OK",
                strlen(content), content);
}

void build_error_response(char *buffer, size_t buffer_size,
                    int status_code, const char *message) {
    const char *status_text;
    switch (status_code) {
    case HTTP_BAD_REQUEST:
        status_text = "Bad Request";
        break;
    case HTTP_NOT_FOUND:
        status_text = "Not Found";
        break;
    case HTTP_INTERNAL_ERROR:
        status_text = "Internal Server Error";
        break;
    default:
        status_text = "Error";
    }
    build_response(buffer, buffer_size, status_code, status_text,
                strlen(message), message);
}


/* ============================================================================
 * 計算機能
 * ========================================================================== */

int parse_calc_query(const char *path, int *v1_p, char *op_p, int *v2_p) {
    int matched;

    if (!path) {
        return -1;
    }

    matched = sscanf(path, "/calc?query=%d%c%d",
                        v1_p, op_p, v2_p);
    if (matched != 3) {
        log_error("Failed to parse calc query: %s", path);
        return -1;
    }
    if (strchr("+-*/", *op_p) == NULL) {
        log_error("Invalid operator: %c", *op_p);
        return -1;
    }

    return 0;
}

int calculate(int v1, char op, int v2, int *result) {
    switch (op)
    {
    case '+':
        *result = v1 + v2;
        break;
    case '-':
        *result = v1 - v2;
        break;
    case '*':
        *result = v1 * v2;
        break;
    case '/':
        if (v2 == 0) {
            log_error("Division by zero");
            return -1;
        }
        *result = v1 / v2;
        break;
    default:
        return -1;
    }

    return 0;
}

int handle_calculator_endpoint(const HTTPRequest *request, char *response, size_t response_size) {
    int v1, v2;
    char op;
    int result;

    if (parse_calc_query(request->path, &v1, &op, &v2) < 0) {
        build_error_response(response, response_size, HTTP_BAD_REQUEST,
                           "Invalid query format. Use: /calc?query=<num><op><num>");
        return -1;
    }

    if (calculate(v1, op, v2, &result) < 0) {
        build_error_response(response, response_size, HTTP_BAD_REQUEST,
                           "Calculation error (e.g., division by zero)");
        return -1;
    }

    build_success_response(response, response_size, result);
    return 0;
}


/* ============================================================================
 * リクエストハンドラ
 * ========================================================================== */

void handle_request(const HTTPRequest *request, int client_socket) {
    char response[MAX_RESPONSE_SIZE];
    ssize_t bytes_written;

    if (strcmp(request->method, "GET") != 0) {
        build_error_response(response, sizeof(response),
                            HTTP_BAD_REQUEST, "Only GET method is supported");
        write(client_socket, response, strlen(response));
        return ;
    }

    if (strncmp(request->path, "/calc?query=", 12) != 0) {
        build_error_response(response, sizeof(response),
                            HTTP_NOT_FOUND, "Not Found. Use: /calc?query=<num><op><num>");
        write(client_socket, response, strlen(response));
        return ;
    }

    handle_calculator_endpoint(request, response, sizeof(response));
    bytes_written = write(client_socket, response, strlen(response));
    if (bytes_written < 0) {
        log_error("Failed to send response: %s", strerror(errno));
    }
}


/* =======================================================================
 * ネットワーク処理
 * ==================================================================== */

int create_server_socket(int port) {
    int server_socket;
    struct sockaddr_in server_address;
    int opt = 1;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        log_exit("Socket creation failed: %s", strerror(errno));
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(server_socket);
        log_exit("Setsockopt failed: %s", strerror(errno));
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;  // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);;  // localhost only
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        close(server_socket);
        log_exit("Bind failed: %s", strerror(errno));
    }

    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        close(server_socket);
        log_exit("Listen failed] %s", strerror(errno));
    }

    return server_socket;
}

char* receive_request(int client_socket) {
    char *buffer = xmalloc(BUFFER_SIZE);

    ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);

    if (bytes_read < 0) {
        log_error("Failed to read from client: %s", strerror(errno));
        free(buffer);
        return NULL;
    }
    if (bytes_read == 0) {
        free(buffer);
        return NULL;
    }
    buffer[bytes_read] = '\0';

    return buffer;
}

void handle_client(int client_socket) {
    char *buffer;
    HTTPRequest *request;

    buffer = receive_request(client_socket);
    if (!buffer) {
        return ;
    }

    request = create_request();
    if (parse_request(request, buffer) != 0) {
        char response[MAX_RESPONSE_SIZE];
        build_error_response(response, sizeof(response),
                            HTTP_BAD_REQUEST, "Invalid HTTP Request");
        write(client_socket, response, strlen(response));
        free(buffer);
        free_request(request);
        return ;
    }
    free(buffer);
    handle_request(request, client_socket);
    free_request(request);
}


/* =======================================================================
 * メインループ
 * ==================================================================== */

void service(int port) {
    int server_socket;
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);

    server_socket = create_server_socket(port);

    client_socket = accept(server_socket,
                            (struct sockaddr *)&client_address,
                            &client_address_length);
    if (client_socket < 0) {
        log_error("Accept failed: %s", strerror(errno));
    }
    handle_client(client_socket);
    close(client_socket);

    close(server_socket);
}


/* =======================================================================
 * メイン関数
 * ==================================================================== */

int main(int argc, char *argv[]) {
    if (argc != 2) {
        log_exit("Missing required arguments.\nUsage: ./http_server <port>");
    }
    if (!(1024 <= atoi(argv[1]) && atoi(argv[1]) < 49152)) {
        log_exit("Invalid port number: %s\nUsage: 1024~49151", argv[1]);
    }

    service(atoi(argv[1]));

    return EXIT_SUCCESS;
}
