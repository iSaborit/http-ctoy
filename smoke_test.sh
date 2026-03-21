#!/usr/bin/env bash
set -euo pipefail

make >/dev/null

server_log="$(mktemp)"

./bin/server >"$server_log" 2>&1 &
server_pid=$!
trap 'kill $server_pid 2>/dev/null || true; wait $server_pid 2>/dev/null || true; rm -f "$server_log"' EXIT

sleep 0.2

request=$'GET /smmmmoke HTTP/1.1\r\nHost: 127.0.0.1:4000\r\nX-Smoke-Test: parse-me\r\nConnection: close\r\n\r\n'
printf "%s" "$request" | nc -w 1 127.0.0.1 4000 >/dev/null || true

kill "$server_pid" 2>/dev/null || true
wait "$server_pid" 2>/dev/null || true

if ! grep -q "GET" "$server_log"; then
    echo
    echo "Smoke test failed: server did not log a parsed request."
    echo "Server log:"
    cat "$server_log"
    exit 1
fi
