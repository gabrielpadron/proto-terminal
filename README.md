# Proto-terminal

This is a terminal prototype implemented in C as a project for the Operating Systems course. The program provides a command-line interface where users can enter commands to interact with the operating system.

## Features
The shell program supports the following features:

1. Changing the current working directory using the `cd` command.
2. Printing the current working directory using the `pwd` command.
3. Listing the files in the current directory using the `ls` command.

Also, the terminal has to be able to use pipeline commands like >, <, | and &.

## Usage
To run the shell program, first compile the code using a C compiler, for example, GCC:
```bash
gcc main.c -o main
```

Then, run the executable:
```bash
./main
```

The shell prompt will appear as follows:
```bash
$
```

You can enter any supported command at the prompt. To exit the shell, use the `exit` command or press `Ctrl + C`.
