#ifndef SERVER_H
#define SERVER_H

#define PORT 30000
#define BUFFER_SIZE 12

int create_listener_socket(void);
int accept_connection(int listener_descriptor);
void handle_client(int connect_descriptor);

#endif