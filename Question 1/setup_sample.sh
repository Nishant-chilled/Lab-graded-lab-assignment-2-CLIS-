#!/usr/bin/env bash

rm -rf submissions backup_unique

mkdir -p submissions/teamA submissions/teamB

printf 'Operating Systems Assignment - Alice\n' \
    > submissions/alice.txt

printf 'Operating Systems Assignment - Bob\n' \
    > submissions/bob.txt

printf 'Operating Systems Assignment - Alice\n' \
    > submissions/alice_copy.txt

printf 'Database Assignment - Carol\n' \
    > submissions/teamA/carol.txt

printf 'Operating Systems Assignment - Bob\n' \
    > submissions/teamB/bob_duplicate.txt

echo "Created five sample submissions: three unique files and two duplicates."