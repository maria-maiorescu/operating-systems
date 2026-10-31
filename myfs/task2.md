Extend the existing myfs C command-line utility to support recursive directory copying and directory movement, using the code discussed in the labs.

All operations must be implemented using POSIX filesystem functions, without invoking external utilities. The existing functionality and options from HW1 must remain operational.

# 1. Command-line interface

The following existing operations must be extended:
```
-c : Copy files and directories (cp).
-m : Move or rename files and directories (mv).
cp/mv -i Confirm before overwriting existing files/folders.
cp/mv -N Do not overwrite existing files/folders.
cp/mv -v Display each copy operation.
```

# 2. Supported options

Only one new option is introduced. All other options retain their meanings from HW1:

`cp -r` Recursively copy directories and their contents.

Directory movement with -m must work recursively without requiring an additional option.

# 3. Functional requirements
* Use `opendir()` and `readdir()` to traverse directories recursively. 
* Skip `.` and `..`, create destination directories as needed, and copy all nested directories and regular files while preserving the directory structure.
* Ensure that empty directories are also copied.
* Support paths containing spaces.
* Prevent infinite recursion by avoiding traversal through symbolic links. If symbolic links are found in the source, symbolic links will be created at the destination that refer to the same file as the original (the indicated file is not copied!). 
* Use `rename()` to move or rename directories whenever possible, including directories containing nested files and subdirectories.
* Never delete the source directory if copying fails. 
* Prevent copying or moving a directory into itself or one of its descendants.
* Apply the existing `-i` and `-N` options to directory movement, ensuring that existing destination data is not overwritten contrary to the selected policy.

# 4. Validation and error handling 
* Reject unknown options, missing operands, incompatible options, nonexistent source directories, and invalid destinations.
* Check the return values of all POSIX filesystem operations and report errors. 
* Handle permission restrictions, inaccessible directories, symbolic links, unsupported filesystem objects, and partial copy failures.
* Release allocated memory and close directory streams and file descriptors appropriately.
* Ensure that failed operations do not accidentally delete or corrupt the original data.

# 5. Examples

Assume the following source directory structure:
```
source/
├── file1.txt 
├── empty/ 
└── subdir/ 
            ├── file2.txt 
            └── nested/ 
                         └── file3.txt
```

`./myfs -c -r source destination`--- Recursively copies the entire source directory structure, including all nested files and empty directories, into destination.

`./myfs -c -r -v source backup` --- Recursively copies source into backup and displays each file and directory copy operation (e.g. source/subdir/file2.txt -> backup/subdir/file2.txt).

`./myfs -c -r -p source backup` --- Recursively copies source into backup while preserving permissions and timestamps where permitted.

`./myfs -c -r -i source destination` --- Recursively copies source and prompts for confirmation before overwriting existing destination files.

`./myfs -c -r -N source destination` --- Recursively copies source without overwriting existing destination files.

`./myfs -m source renamed_source` --- Renames or moves the entire source directory, including all nested files and subdirectories, to renamed_source.

`./myfs -m -v source destination` --- Moves the entire directory and displays the operation (e.g. source -> destination).

`./myfs -m -N source destination` --- Moves the source directory only if the destination does not already exist; otherwise, leaves the existing destination unchanged.

`./myfs -m -i source destination` --- Requests confirmation before an operation that would overwrite an existing destination, where overwriting is supported.

