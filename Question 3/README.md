# Question 3 — Employee Record File Processing

## Objective

The program uses Linux/POSIX system calls to:

1. Create a file.
2. Write fixed-size employee records.
3. Update a selected record directly.
4. Retrieve a record from any location efficiently.

## Command 1: Compile the program

Linux:

```bash
gcc -Wall -Wextra -std=c11 employee_records.c -o employee_records 2> compile_errors.txt
```

macOS:

```bash
clang -Wall -Wextra -std=c11 employee_records.c -o employee_records 2> compile_errors.txt
```

The command compiles the program and stores compiler diagnostics in `compile_errors.txt`.

## Command 2: Execute the program

```bash
./employee_records > output.txt 2> runtime_errors.txt
```

The program creates the employee data file, writes three records, updates one salary, and retrieves another record directly.

## Command 3: Inspect the generated file

```bash
ls -l employee_records.dat > file_metadata.txt
cat file_metadata.txt
```

The file should have permissions similar to:

```text
-rw-------
```

The exact file size, user, group, and timestamp depend on the operating system.

## Command 4: Display the program output

```bash
cat output.txt
```

Expected result:

```text
Created employee_records.dat and wrote 3 fixed-size employee records.
Before update: id=102, name=Meera Das, salary=62000.00
After update: id=102, name=Meera Das, salary=67500.00
Retrieved record at index 2: id=103, name=Rohan Singh, salary=58000.00
File descriptor closed successfully.
```

## Contribution of the system calls

### `open()`

```c
open(
    DATA_FILE,
    O_CREAT | O_TRUNC | O_RDWR,
    S_IRUSR | S_IWUSR
);
```

`open()` creates a new read/write file and returns a file descriptor. The mode gives read and write access only to the owner.

### `write()`

`write()` stores each employee structure in the file. The helper function handles interruptions and partial writes.

### `lseek()`

```c
record_index * sizeof(Employee)
```

`lseek()` moves the current file offset directly to a selected record. Therefore, a record can be updated without rewriting all earlier records.

### `read()`

`read()` retrieves the selected employee record from the current position.

### `close()`

`close()` releases the file descriptor and associated kernel resources after processing is complete.

Fixed-size records make direct access efficient because the position of any record can be calculated immediately.

## Files generated after execution

- `employee_records`
- `employee_records.dat`
- `compile_errors.txt`
- `runtime_errors.txt`
- `output.txt`
- `file_metadata.txt`

## Required screenshots

Created a `screenshots` folder and save:

- `01_compilation.png`
- `02_program_output.png`
- `03_file_metadata.png`
- `04_runtime_check.png`
