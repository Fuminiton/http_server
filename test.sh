#!/bin/bash

PORT=8080
IP_ADDRESS="127.0.0.1"
SERVER_BIN="./http_server"
CLIENT_BIN="./http_client"

assert() {
    local expected="$1"
    local request="$2"
    
    actual=$(printf "%s\r\n\r\n" "$request" | $CLIENT_BIN $IP_ADDRESS $PORT 2>/dev/null)

    if [ "$expected" == "$actual" ]; then
        echo "PASS: $request"
    else
        echo "FAILED: $request"
        echo "--- Expected ---"
        echo "$expected"
        echo "--- Actual ---"
        echo "$actual"
        exit 1
    fi
}

echo "--- Starting Server on port $PORT ---"

$SERVER_BIN $PORT &
SERVER_PID=$!
trap "echo '--- Killing Server (PID: $SERVER_PID) ---'; kill $SERVER_PID" EXIT
sleep 1

# test
assert "HTTP/1.1 200 OK"$'\r\n'"Content-Length: 2"$'\r\n\r\n'"12" "GET /calc?query=10+2 HTTP/1.1"
assert "HTTP/1.1 200 OK"$'\r\n'"Content-Length: 1"$'\r\n\r\n'"8" "GET /calc?query=10-2 HTTP/1.1"
assert "HTTP/1.1 200 OK"$'\r\n'"Content-Length: 2"$'\r\n\r\n'"24" "GET /calc?query=3*8 HTTP/1.1"
assert "HTTP/1.1 200 OK"$'\r\n'"Content-Length: 2"$'\r\n\r\n'"50" "GET /calc?query=100/2 HTTP/1.1"

echo "All tests passed!"
