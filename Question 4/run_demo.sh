#!/usr/bin/env bash

LOG_FILE="sample_app.log"
REPORT_FILE="error_report.log"
MONITOR_ERROR_FILE="monitor_errors.log"
OUTPUT_FILE="monitor_output.txt"

# Empty previous demonstration files.
: > "$LOG_FILE"
: > "$REPORT_FILE"
: > "$MONITOR_ERROR_FILE"
: > "$OUTPUT_FILE"

./monitor_errors.sh \
    "$LOG_FILE" \
    "$REPORT_FILE" \
    "$MONITOR_ERROR_FILE" \
    > "$OUTPUT_FILE" 2>&1 &

MONITOR_PID=$!

sleep 1

echo "2026-07-24 20:10:01 INFO  Server started" \
    >> "$LOG_FILE"

sleep 1

echo "2026-07-24 20:10:02 ERROR Database connection failed" \
    >> "$LOG_FILE"

sleep 1

echo "2026-07-24 20:10:03 WARN  Retrying connection" \
    >> "$LOG_FILE"

sleep 1

echo "2026-07-24 20:10:04 ERROR Retry limit exceeded" \
    >> "$LOG_FILE"

sleep 1

kill "$MONITOR_PID" 2>/dev/null
wait "$MONITOR_PID" 2>/dev/null || true

echo "Demo completed."
echo "Live output: $OUTPUT_FILE"
echo "ERROR report: $REPORT_FILE"