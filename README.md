# Linux Mini Shell

A Unix-style shell implemented in C as a systems programming project for *COP 4610 — Operating Systems Principles* course at Florida International University.

## Project Overview

This project demonstrates core operating systems concepts through the implementation of a lightweight command-line shell. The shell supports external command execution, built-in commands, input/output redirection, and a single pipeline between processes.

## Features

- Execute external commands using `fork()`, `execvp()`, and `wait()`
- Built-in commands: `cd`, `pwd`, and `exit`
- Output redirection with `>`
- Append redirection with `>>`
- Input redirection with `<`
- Single pipeline support with `|`
- Error handling for invalid commands, directories, files, and process creation
- Linux process and system-call analysis using `strace`, `ps`, and `/proc`

## Academic Context and Attribution

This project was developed as part of *COP 4610 — Operating Systems Principles* at Florida International University. The course provided starter scaffolding, including parts of the shell loop, input-parsing logic, build configuration, and assignment structure. My implementation focused on:

- External command execution
- Built-in shell commands
- Input and output redirection
- Pipe creation and process communication
- Error handling
- Process and system-call analysis
