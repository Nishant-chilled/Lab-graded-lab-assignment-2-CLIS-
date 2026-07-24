# Question 2 — Child Process Monitoring

## Objective

The program:

1. Creates child processes using `fork()`.
2. Monitors their execution using `waitpid()`.
3. Prevents zombie processes.
4. Terminates unresponsive children using signals.

## Command 1: Compile the program

Linux:

```bash
gcc -Wall -Wextra -std=c11 process_monitor.c -o process_monitor 2> compile_errors.txt
```

macOS:

```bash
clang -Wall -Wextra -std=c11 process_monitor.c -o process_monitor 2> compile_errors.txt
```

The compiler converts the C source into an executable. `-Wall -Wextra` enables warnings, and `2>` stores compiler diagnostics in `compile_errors.txt`.

## Command 2: Run the program

```bash
./process_monitor > output.txt 2> runtime_errors.txt
```

The program creates three children. Two finish normally, while the third deliberately runs too long and ignores `SIGTERM`.

## Command 3: Display the result

```bash
cat output.txt
```

The output displays the child PIDs, progress, signal delivery, and termination status. PID values and the exact ordering may change on every execution.

## Command 4: Check errors

```bash
cat compile_errors.txt
cat runtime_errors.txt
```

Both files should normally be empty. Compiler and runtime diagnostics are stored separately from standard output.

## How process creation, waiting, and signals work together

`fork()` creates a new process. It returns zero inside the child and returns the child's PID inside the parent.

The parent stores the PID and starting time of every child. It uses:

```c
waitpid(pid, &status, WNOHANG)
```

to check whether the child has finished without blocking the entire monitoring loop.

When a child exits, `waitpid()` collects its termination information. This removes the terminated child from the kernel process table and prevents it from becoming a zombie.

After five seconds, the parent sends:

```c
kill(pid, SIGTERM);
```

`SIGTERM` requests graceful termination. A process can catch or ignore this signal.

If the process remains active after the grace period, the parent sends:

```c
kill(pid, SIGKILL);
```

`SIGKILL` cannot be caught or ignored and therefore guarantees termination.

The parent then calls `waitpid()` again to reap the terminated child.

## Typical output

```text
Parent PID=<varies> creating 3 child processes.
Parent created child 1 with PID=<varies>.
Parent created child 2 with PID=<varies>.
Parent created child 3 with PID=<varies>.
...
PID=<varies> exceeded 5 seconds; sending SIGTERM.
PID=<varies> ignored SIGTERM; sending SIGKILL.
Parent reaped PID=<varies>; terminated by signal 9.
All child processes have been monitored and reaped.
```

## Files generated after execution

- `process_monitor`
- `compile_errors.txt`
- `output.txt`
- `runtime_errors.txt`

## Required screenshots

Created a `screenshots` folder and save:

- `01_compilation.png`
- `02_process_output.png`
- `03_runtime_check.png`
