#!/bin/bash

PORT=8080
SERVER_BIN="./http_server"
CLIENT_BIN="./http_client"

assert() {
    local expected="$1"
    local client_input="$2"
    
    actual=$($CLIENT_BIN 127.0.0.1 $PORT "$client_input")

    if [ "$expected" = "$actual" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

echo "--- Starting Server on port $PORT ---"

$SERVER_BIN $PORT &
SERVER_PID=$!
trap "echo '--- Killing Server (PID: $SERVER_PID) ---'; kill $SERVER_PID" EXIT
sleep 1

# test start
assert "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\n12\n" "GET /calc?query=10+2 HTTP/1.1"

echo OK