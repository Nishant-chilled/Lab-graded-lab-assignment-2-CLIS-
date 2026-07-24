# Question 4 — Real-Time Log Monitoring

## Objective

The solution:

1. Displays newly added log entries in real time.
2. Extracts lines containing `ERROR`.
3. Maintains a separate error report.
4. Suppresses unnecessary output.

## Command 1: Make the scripts executable

```bash
chmod +x monitor_errors.sh run_demo.sh
```

This gives execute permission to both scripts.

## Command 2: Run the controlled demonstration

```bash
./run_demo.sh
```

The script starts the monitor, adds four sample log entries, and then stops the monitor automatically.

## Command 3: Display the monitored output

```bash
cat monitor_output.txt
```

Expected result:

```text
Monitoring sample_app.log. Press Ctrl+C to stop.
2026-07-24 20:10:01 INFO  Server started
2026-07-24 20:10:02 ERROR Database connection failed
2026-07-24 20:10:03 WARN  Retrying connection
2026-07-24 20:10:04 ERROR Retry limit exceeded
```

## Command 4: Display the error report

```bash
cat error_report.log
```

Expected result:

```text
2026-07-24 20:10:02 ERROR Database connection failed
2026-07-24 20:10:04 ERROR Retry limit exceeded
```

Only entries containing `ERROR` are stored in the report.

## Command 5: Start continuous monitoring manually

```bash
./monitor_errors.sh sample_app.log error_report.log monitor_errors.log
```

This continuously waits for new entries. Press `Ctrl+C` to stop monitoring.

In another terminal, test it with:

```bash
echo "INFO: Application running" >> sample_app.log
echo "ERROR: Connection failed" >> sample_app.log
```

The first line is displayed only. The second line is displayed and also stored in `error_report.log`.

## Pipeline explanation

The principal pipeline is:

```bash
tail -n 0 -F sample_app.log 2>> monitor_errors.log |
    tee >(
        grep --line-buffered 'ERROR' \
            >> error_report.log 2>/dev/null
    )
```

### `tail`

`tail -n 0 -F` ignores old entries and waits for newly appended lines. `-F` also continues following the file if it is replaced during log rotation.

### Pipe

The pipe sends the output of `tail` directly into `tee`. No temporary intermediate file is required.

### `tee`

`tee` keeps all entries visible on standard output and sends a copy into the `grep` branch.

### `grep`

`grep --line-buffered 'ERROR'` selects only entries containing the word `ERROR`. Line buffering prevents unnecessary delays during continuous monitoring.

### Redirection

```bash
>> error_report.log
```

appends matched entries without deleting older report contents.

```bash
2>> monitor_errors.log
```

stores diagnostics from `tail` separately.

### `/dev/null`

```bash
2>/dev/null
```

discards unnecessary `grep` diagnostics. `/dev/null` acts as a data sink.

The solution is efficient because each new log line is processed only once as it arrives.

## Files generated after execution

- `sample_app.log`
- `error_report.log`
- `monitor_errors.log`
- `monitor_output.txt`

## Required screenshots

Created a `screenshots` folder and save:

- `01_permissions.png`
- `02_demo_execution.png`
- `03_live_output.png`
- `04_error_report.png`
- `05_error_check.png`
