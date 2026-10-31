Implement a C command-line utility named myfs that provides file operations similar to Unix touch, cat, cp, and mv. All operations must be implemented in a single executable using POSIX system calls, without invoking external utilities.

# 1. Command-line interface

The operation must be selected using one of the following options:
```
 -t : Create files or modify timestamps (touch)
 -a : Display file contents (cat)
 -c : Copy files (cp)
 -m : Move or rename files (mv)
```
# 2. Supported options
```
Operation    Options    Description
touch -C         Do not create nonexistent files.

cat -n          Number all lines.
    -b          Number non-empty lines only.
    -E          Mark line endings with $.
    -S          Display tabs as ^I.                   
    -s          Suppress consecutive empty lines into a single one.

cp  -i         Confirm before overwriting.
    -N         Do not overwrite existing files.
    -v          Display each copy operation.
    -p          Preserve permissions and timestamps.
    
mv  -i          Confirm before overwriting.
    -N         Do not overwrite existing files.
    -v          Display each move operation.
```
# 3. Functional requirements

- *Touch*: Create files when necessary.

- *Cat*: Display one or more files in the specified order. If no file is provided, read from standard input. Process content incrementally to support large files.

- *Cp*: Copy regular files, create destinations when necessary, handle overwriting according to the selected options, and correctly handle partial writes.

- *Mv*: Move or rename files.

# 4. Validation and error handling

Reject unknown options, missing operands, multiple operation selectors, and incompatible options. Check system-call return values, report errors clearly, and release any used resources/memory appropriately.

# 5. Examples

`./myfs -t file.txt`    ---  Creates file.txt if it does not exist; otherwise, updates its access and modification timestamps to the current time.

`./myfs -a -n file.txt`    ---    Displays the contents of file.txt and numbers every output line, including empty lines.

`./myfs -a -E file1.txt file2.txt`  ---    Displays the contents of file1.txt, followed by file2.txt, adding $ at the end of each line.

`./myfs -c -v source.txt destination.txt`  ---    Copies source.txt to destination.txt and displays the source and destination paths (e.g. <path_to_source>/source.txt -> <path_to_destination>/destination.txt).

`./myfs -c -i source.txt destination.txt`  ---    Copies the file but prompts for confirmation if destination.txt already exists.

`./myfs -c -p source.txt backup.txt`  ---    Copies source.txt to backup.txt, preserving its permissions and timestamps where permitted.

`./myfs -m -v oldname.txt newname.txt` ---    Renames or moves oldname.txt to newname.txt and displays the operation (e.g. <path_to_source>/oldname.txt -> <path_to_destination>/newname.txt)

`./myfs -m -N source.txt destination.txt`  ---    Moves source.txt to destination.txt only if the destination does not already exist; otherwise, leaves the existing destination unchanged.


# 6. Deliverables

The deliverables will consist of a single C source code file.
