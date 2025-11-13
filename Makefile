CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Isrc/common

SRC_DIR = src
COMMON_DIR = $(SRC_DIR)/common
SERVER_DIR = $(SRC_DIR)/server
BUILD_DIR = build

COMMON_SRCS = $(COMMON_DIR)/utils.c
SERVER_SRCS = $(SERVER_DIR)/main.c $(SERVER_DIR)/server.c

COMMON_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(COMMON_SRCS))
SERVER_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SERVER_SRCS))

SERVER_TARGET = server

.PHONY: all server clean run-server

all: $(SERVER_TARGET)

server: $(SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(SERVER_TARGET}

run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)
