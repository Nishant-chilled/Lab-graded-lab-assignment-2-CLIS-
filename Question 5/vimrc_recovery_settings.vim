" Enable swap-file recovery.
set swapfile

" Keep a backup of the previous saved file.
set backup

" Protect the file while it is being written.
set writebackup

" Keep undo history after Vim closes.
set undofile

" Recovery-storage directories.
set directory^=./vim-recovery-data/swap//
set backupdir^=./vim-recovery-data/backup//
set undodir^=./vim-recovery-data/undo//

" Write recovery information regularly.
set updatecount=100
set updatetime=4000