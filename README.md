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
