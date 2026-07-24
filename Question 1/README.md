# Question 1 — Duplicate Submission Backup

## Objective

The shell script:

1. Detects duplicate files using SHA-256 checksums.
2. Copies only unique files into a backup directory.
3. Generates a processing report.
4. Stores error messages separately.

## Command 1: Make the scripts executable

```bash
chmod +x setup_sample.sh duplicate_backup.sh
```

This command adds execute permission to the scripts. They can then be executed directly using `./script-name.sh`.

## Command 2: Create sample submissions

```bash
./setup_sample.sh > setup_output.txt 2> setup_errors.log
```

The script creates five sample submission files. Three contain unique content, while two intentionally duplicate existing files.

## Command 3: Run the duplicate detector

```bash
./duplicate_backup.sh submissions backup_unique > output.txt 2>> errors.log
```

The first argument specifies the source folder and the second specifies the backup folder. Standard output is stored in `output.txt`, while standard error is appended to `errors.log`.

## Command 4: Display the generated report

```bash
cat report.txt
```

This displays the number of files processed, duplicates detected, and unique files backed up.

Expected report:

```text
Submission Processing Report
============================
Source directory : submissions
Backup directory : backup_unique
Files processed  : 5
Duplicates found : 2
Unique files backed up: 3
```

## Command 5: Display duplicate details

```bash
cat duplicates.txt
```

This displays which files have identical contents. The order and the file treated as the original may differ because directory traversal order is not guaranteed.

## Command 6: Verify the backup

```bash
find backup_unique -type f | sort > backup_files.txt
cat backup_files.txt
```

`find` lists all backed-up regular files. Only three files should appear because only unique submissions are copied.

## Command justification

- `find -print0` handles filenames containing spaces safely.
- `sha256sum` or `shasum -a 256` compares files using their contents.
- `awk` extracts the checksum and searches the temporary checksum index.
- `mkdir -p` creates the backup directory and nested folders.
- `cp` copies the unique submission files.
- `>` creates or overwrites a file.
- `>>` appends data without deleting existing content.
- `2>` and `2>>` redirect standard error separately from normal output.
- `tee` displays an error and stores it simultaneously.

## Files generated after execution

- `submissions/`
- `backup_unique/`
- `setup_output.txt`
- `setup_errors.log`
- `output.txt`
- `report.txt`
- `duplicates.txt`
- `backup_files.txt`
- `errors.log`

## Required screenshots

Created a folder called `screenshots` and save screenshots such as:

- `01_permissions.png`
- `02_sample_setup.png`
- `03_script_execution.png`
- `04_report_and_duplicates.png`
- `05_backup_files.png`
