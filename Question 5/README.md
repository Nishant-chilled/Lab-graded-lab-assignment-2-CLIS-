# Question 5 — Controlled Vim Recovery Demonstration

Use only the supplied sample `critical.conf` for this demonstration. Do not perform the crash test on an actual system configuration.

## Command 1: Create recovery directories

```bash
mkdir -p vim-recovery-data/swap
mkdir -p vim-recovery-data/backup
mkdir -p vim-recovery-data/undo
```

These commands create locations for swap, backup, and persistent undo files.

## Command 2: Preserve the original file

```bash
cp -p critical.conf critical.conf.before-crash
```

`cp -p` creates a rollback copy and preserves available file metadata.

## Command 3: Open the sample configuration

```bash
vim critical.conf
```

Inside Vim:

1. Press `i`.
2. Change:

```text
max_connections=100
```

to:

```text
max_connections=250
```

3. Do not save the file.
4. Keep Vim open.

This creates unsaved data that can be recovered from the swap file.

## Command 4: Find Vim's PID in another terminal

Linux:

```bash
pgrep -a vim
```

macOS:

```bash
pgrep -l vim
```

This displays the process ID of the active Vim process.

## Command 5: Simulate a crash

```bash
kill -9 VIM_PID
```

Replace `VIM_PID` with the PID shown by the previous command. `SIGKILL` immediately terminates the sample Vim process and leaves the recovery swap file behind.

## Command 6: List the files after the crash

```bash
ls -la > file_listing_after_crash.txt
cat file_listing_after_crash.txt
```

The listing should contain a hidden swap file similar to:

```text
.critical.conf.swp
```

## Command 7: List recoverable Vim sessions

```bash
vim -r > recoverable_sessions.txt 2>&1
cat recoverable_sessions.txt
```

`vim -r` searches for swap files and lists recoverable editing sessions. `2>&1` places both normal messages and errors in the same output file.

## Command 8: Recover the file

```bash
vim -r critical.conf
```

Inside Vim, save the recovered buffer under a new name:

```vim
:w critical.conf.recovered
:q
```

The recovered version is saved separately so the original file remains protected.

## Command 9: Compare the versions

```bash
diff -u critical.conf.before-crash critical.conf.recovered > recovery_diff.txt
cat recovery_diff.txt
```

The output should show the recovered unsaved change:

```diff
-max_connections=100
+max_connections=250
```

This confirms that the swap file successfully restored the unsaved content.

## Command 10: Remove the stale swap last

```bash
rm -i .critical.conf.swp
```

The `-i` option requests confirmation before deletion. The swap should be removed only after the recovered file has been checked.

## Evaluation summary

- Swap files are the primary source for unsaved edits.
- Persistent undo restores previous edit states but may not contain the newest unsaved buffer.
- Registers can restore fragments but not reliably recover the full file.
- Backup files contain the previous saved version.
- Auto-recovery detects swap files and presents recovery choices.

The safest method is to preserve all versions, recover from the swap, save under a new filename, compare the versions, validate the configuration, and delete the swap only after recovery is complete.

## Files generated during the demonstration

- `critical.conf.before-crash`
- `.critical.conf.swp`
- `file_listing_after_crash.txt`
- `recoverable_sessions.txt`
- `critical.conf.recovered`
- `recovery_diff.txt`
- `vim-recovery-data/`

## Required screenshots

Created a `screenshots` folder and save:

- `01_recovery_setup.png`
- `02_unsaved_vim_edit.png`
- `03_simulated_crash.png`
- `04_swap_file.png`
- `05_recoverable_session.png`
- `06_recovery_difference.png`
