#!/usr/bin/env bash

LOG_FILE="${1:-sample_app.log}"
REPORT_FILE="${2:-error_report.log}"
MONITOR_ERROR_FILE="${3:-monitor_errors.log}"
PIPE_FILE=".monitor_pipe_$$"

touch "$LOG_FILE" "$REPORT_FILE" "$MONITOR_ERROR_FILE"

if ! mkfifo "$PIPE_FILE"; then
    echo "Error: could not create monitoring pipe." \
        >> "$MONITOR_ERROR_FILE"
    exit 1
fi

TAIL_PID=""
READER_PID=""

cleanup() {
    trap - INT TERM EXIT

    if [ -n "$TAIL_PID" ]; then
        kill "$TAIL_PID" 2>/dev/null || true
        wait "$TAIL_PID" 2>/dev/null || true
    fi

    if [ -n "$READER_PID" ]; then
        kill "$READER_PID" 2>/dev/null || true
        wait "$READER_PID" 2>/dev/null || true
    fi

    rm -f "$PIPE_FILE"
}

trap 'cleanup; exit 0' INT TERM
trap cleanup EXIT

echo "Monitoring $LOG_FILE. Press Ctrl+C to stop."

tail -n 0 -F "$LOG_FILE" \
    2>> "$MONITOR_ERROR_FILE" > "$PIPE_FILE" &

TAIL_PID=$!

while IFS= read -r line; do
    # Display every newly added log entry.
    printf '%s\n' "$line"

    # Store only ERROR entries in the report.
    printf '%s\n' "$line" |
        grep 'ERROR' >> "$REPORT_FILE" 2>/dev/null || true

done < "$PIPE_FILE" &

READER_PID=$!

wait "$READER_PID"