#!/usr/bin/env bash

# Identifies duplicate submissions by file content, backs up only unique files,
# creates a summary report, and stores errors separately.

SOURCE_DIR="${1:-submissions}"
BACKUP_DIR="${2:-backup_unique}"
DUPLICATE_FILE="duplicates.txt"
REPORT_FILE="report.txt"
ERROR_FILE="errors.log"
HASH_INDEX=".hash_index.tmp"

# Start each execution with fresh result files.
: > "$DUPLICATE_FILE"
: > "$REPORT_FILE"
: > "$ERROR_FILE"
: > "$HASH_INDEX"

cleanup() {
    rm -f "$HASH_INDEX"
}

trap cleanup EXIT

if [ ! -d "$SOURCE_DIR" ]; then
    echo "Error: source directory '$SOURCE_DIR' does not exist." |
        tee -a "$ERROR_FILE" >&2
    exit 1
fi

if ! mkdir -p "$BACKUP_DIR" 2>> "$ERROR_FILE"; then
    echo "Error: could not create backup directory '$BACKUP_DIR'." \
        >> "$ERROR_FILE"
    exit 1
fi

# Linux normally provides sha256sum.
# macOS normally provides shasum.
calculate_hash() {
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$1" | awk '{print $1}'
    elif command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$1" | awk '{print $1}'
    else
        return 1
    fi
}

if ! command -v sha256sum >/dev/null 2>&1 &&
   ! command -v shasum >/dev/null 2>&1; then
    echo "Error: neither sha256sum nor shasum is installed." \
        >> "$ERROR_FILE"
    exit 1
fi

processed=0
duplicated=0
backed_up=0

# -print0 safely handles filenames containing spaces.
while IFS= read -r -d '' file; do
    processed=$((processed + 1))

    hash=$(calculate_hash "$file" 2>> "$ERROR_FILE")

    if [ -z "$hash" ]; then
        echo "Could not calculate checksum for: $file" >> "$ERROR_FILE"
        continue
    fi

    relative_path="${file#"$SOURCE_DIR"/}"

    original=$(
        awk -F '\t' -v value="$hash" \
            '$1 == value {print $2; exit}' "$HASH_INDEX"
    )

    if [ -n "$original" ]; then
        duplicated=$((duplicated + 1))

        printf '%s -> duplicate of %s\n' \
            "$relative_path" "$original" >> "$DUPLICATE_FILE"
    else
        printf '%s\t%s\n' \
            "$hash" "$relative_path" >> "$HASH_INDEX"

        destination="$BACKUP_DIR/$relative_path"

        if mkdir -p "$(dirname "$destination")" 2>> "$ERROR_FILE" &&
           cp "$file" "$destination" 2>> "$ERROR_FILE"; then
            backed_up=$((backed_up + 1))
        else
            echo "Could not back up: $file" >> "$ERROR_FILE"
        fi
    fi

done < <(find "$SOURCE_DIR" -type f -print0 2>> "$ERROR_FILE")

{
    echo "Submission Processing Report"
    echo "============================"
    echo "Source directory : $SOURCE_DIR"
    echo "Backup directory : $BACKUP_DIR"
    echo "Files processed  : $processed"
    echo "Duplicates found : $duplicated"
    echo "Unique files backed up: $backed_up"
} > "$REPORT_FILE"

cat "$REPORT_FILE"

if [ "$duplicated" -gt 0 ]; then
    echo
    echo "Duplicate details:"
    cat "$DUPLICATE_FILE"
fi

if [ -s "$ERROR_FILE" ]; then
    echo "Some errors occurred. Check $ERROR_FILE." >&2
fi