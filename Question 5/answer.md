# Question 5 — vi/Vim Crash-Recovery Evaluation

When the system crashes before `:w` is used, the original configuration file normally contains the last successfully saved version. The newest unsaved edits are usually recovered from Vim's swap file.

## 1. Swap files

Vim normally creates a hidden swap file such as:

```text
.critical.conf.swp
```

The swap file periodically stores the current editing-buffer state. It is the main recovery mechanism for restoring unsaved changes after Vim or the operating system crashes.

Recovery can be started using:

```bash
vim -r critical.conf
```

The swap file usually contains the newest recoverable version. However, the last few keystrokes may be missing if Vim had not yet written them to the swap.

## 2. Undo history

Normal undo history is maintained in memory while Vim is running. It is generally lost after a crash unless persistent undo is enabled:

```vim
set undofile
```

Persistent undo allows the user to move through previous editing states after reopening a file. However, it is not a complete replacement for swap recovery because the most recent unsaved buffer may not have been written to the undo file.

## 3. Registers

Vim registers store text that has been copied, deleted, or changed.

Examples include:

```vim
:registers
"0p
"1p
```

Registers can recover individual pieces of text during an active session. They are not a reliable full-file recovery method because they generally contain fragments rather than a complete and consistent copy of the editing buffer.

## 4. Backup files

If the following settings are enabled:

```vim
set backup
set writebackup
```

Vim can create a backup file such as:

```text
critical.conf~
```

This file normally contains the version that existed before the latest successful write.

A backup file is useful when a saved file becomes corrupted or an incorrect version is written. However, it usually does not contain edits that had never been saved before the crash.

## 5. Auto-recovery

When Vim detects an existing swap file while opening the original file, it displays a warning and offers options such as:

- Open read-only
- Edit anyway
- Recover
- Quit
- Delete the swap

This automatic swap detection prevents Vim from silently ignoring recoverable data.

The swap file should not be deleted until it is confirmed that no other Vim process is still editing the file.

## Most reliable recovery strategy

The most reliable strategy is:

1. Check whether another Vim process is still editing the file.
2. Preserve the original file, swap file, backup file, and undo file.
3. Recover using `vim -r critical.conf` or Vim's `R` recovery option.
4. Save the recovered buffer under a new filename.
5. Compare the recovered file with the original and backup.
6. Validate the recovered configuration using the application's syntax checker.
7. Keep a rollback copy before replacing the original.
8. Delete the stale swap file only after successful recovery.

A suitable recovery command is:

```bash
vim -r critical.conf
```

Inside Vim, save the recovered buffer using:

```vim
:w critical.conf.recovered
:q
```

The original and recovered files can then be compared using:

```bash
diff -u critical.conf critical.conf.recovered
```

This is the most reliable strategy because the swap file provides the newest likely version, while the original and backup files provide safe fallback versions. Saving under a new name prevents accidental destruction of the original configuration.
