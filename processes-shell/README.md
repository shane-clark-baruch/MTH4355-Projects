# 🖥️ OS Process Shell — CS380 Project

_Author: Marvin Chaudhary (G / Marv)_  
_Class: CS380 Operating Systems_  
_Instructor: Dr. Shuja_

---

## 📌 Overview

This project is an implementation of a miniature Unix-like **shell** in C, following the “processes-shell” specification from the OSTEP project library.

It demonstrates key Operating System concepts including:

- **Process Creation** (`fork()`)
- **Program Loading** (`execv()`)
- **Process Synchronization** (`waitpid()`)
- **Pipelines & Redirection** (`dup2()` & file descriptors)
- **Job Control** (background jobs, `fg`, `bg`)
- **Process Groups** (`setpgid()`, `tcsetpgrp()`)
- **Signals** (`SIGCHLD`, `SIGINT`, `SIGTSTP`, `SIGTTOU`)
- **Terminal Control**
- **Parsing & Tokenization**

This shell supports:

- Running commands with arguments
- Input redirection (`<`)
- Output redirection (`>`, `>>`)
- Pipelines (`cmd1 | cmd2 | cmd3`)
- Background execution (`&`)
- Job management (`jobs`, `fg %n`, `bg %n`)
- Quoted strings (`"hello world"` and `'hello'`)
- Syntax error detection
- Ctrl-Z to suspend jobs
- Ctrl-C ignored in shell (but works in child processes)

---

## 📂 Project Structure

```
processes-shell/
│
├── include/
│   ├── shell.h       # parser + executor interface
│   ├── jobs.h        # background job subsystem
│
├── src/
│   ├── shell.c       # main REPL loop + builtins + signal handling
│   ├── parser.c      # tokenizer + command parser (Step 4)
│   ├── exec.c        # execution engine (pipelines + pgroups + redirection)
│   ├── jobs.c        # job tracking + SIGCHLD reaping
│
├── tests/
│   └── demo.sh       # automated demonstration script
│
├── Makefile
└── shell            # compiled executable after running make
```

---

## 🚀 Build Instructions

### Requirements (Tested On)

- macOS (Intel)
- `clang` or `gcc`
- Standard Unix utilities (`ls`, `cat`, `wc`)

### To Build:

```bash
make clean
make
```

### To Run:

```bash
./shell
```

---

## 🧪 Supported Features & Examples

### ▶ Basic commands

```
osh> ls
osh> pwd
```

### ▶ Redirection

```
osh> echo "hello" > file.txt
osh> cat < file.txt
```

### ▶ Pipelines

```
osh> echo "hello world" | tr a-z A-Z | wc -w
```

### ▶ Background jobs

```
osh> sleep 10 &
[bg] 34567
```

### ▶ Listing jobs

```
osh> jobs
[1] 34567 Running (sleep 10)
```

### ▶ Bringing job to foreground

```
osh> fg %1
```

### ▶ Resuming job in background

```
osh> bg %1
```

### ▶ Stopping a job (Ctrl+Z)

```
osh> sleep 20
^Z
[Stopped]
```

---

## 🔍 Syntax Error Handling

Examples:

```
osh> |
osh: syntax error near unexpected token '|'

osh> ls >
osh: missing filename after '>'

osh> "unterminated
osh: unmatched double quote
```

---

## 📚 OS Concepts Used (Mapped to Class Topics)

### ✔ Process API

- `fork()` to create child processes
- `execv()` to load programs

### ✔ Process Control / Scheduling

- Foreground vs Background execution
- Process groups
- Terminal control

### ✔ Synchronization

- `waitpid()` with `WUNTRACED`, `WCONTINUED`, `WNOHANG`

### ✔ Signals

- `SIGCHLD` for reaping children
- `SIGINT` ignored by shell
- `SIGTSTP` suspend jobs
- `SIGTTOU` ignored for shell (job control safety)

### ✔ Virtualization (OS Abstraction)

- Each command becomes a separate virtualized process
- Each pipeline becomes a virtualized "process group"

---

## 🎥 Demo Script (tests/demo.sh)

Run automatic demonstration:

```bash
bash tests/demo.sh
```

This script shows:

- pipelines
- redirection
- background/foreground jobs
- quoted arguments
- error handling

---

## 📝 What I Learned

- How process creation works internally
- How Unix job control is implemented in terminals
- How shells manage multiple processes in pipelines
- How signals coordinate events between parent/child processes
- How redirection & pipes rely on file descriptor manipulation
- How to build a safe parser for shell syntax

---

## 📌 Acknowledgment

Project inspired by the OSTEP (Operating Systems: Three Easy Pieces) project set.
