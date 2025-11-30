CFLAGS = -Wall -Wextra -std=c11 -pedantic
LDFLAGS =

SERVER = http_server
SERVER_SRCS = server.c
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

CLIENT = http_client
CLIENT_SRCS = client.c
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER) $(CLIENT)

server: $(SERVER)
client: $(CLIENT)

test: $(SERVER) $(CLIENT)
	./test.sh

.PHONY: all clean server client test