
# Parallel Sed

In an earlier project, you implemented a simple compression tool based on
run-length encoding, known simply as `sed`. Here, you'll implement something
similar, except you'll use threads to make a parallel version of `sed`. We'll
call this version ... wait for it ... `psed`. 

There are three specific objectives to this assignment:

* To familiarize yourself with the Linux pthreads.
* To learn how to parallelize a program.
* To learn how to program for high performance.

## Background

To understand how to make progress on this project, you should first
understand the basics of thread creation, and perhaps locking and signaling
via mutex locks and condition variables. These are described in the following
book chapters:

- [Intro to Threads](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
- [Threads API](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf)
- [Locks](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks.pdf)
- [Using Locks](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf)
- [Condition Variables](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf)

Read these chapters carefully in order to prepare yourself for this project.

## Overview

First, recall how `wsed` works by reading the description
[here](https://github.com/shane-clark-baruch/MTH4355-Projects/tree/master/project-1-unix). 
You'll use the same basic specification with translation and substitution components of the stream 

Your parallel sed (`psed`) will externally look the same, but we have added an additional flag. The general usage
from the command line will be as follows:

```.sh
prompt> ./wsed -s 16384 -m translation ab ba test.txt
```

The flag `-s 16000` specifies that each thread with edit a chunk of size `16000` bytes. For example, if your document is `320000`-bytes,
then you will break up this into 20 chunks of size 16000-bytes. Each of these chunks should be edited by one thread. 


## Considerations

Doing so effectively and with high performance will require you to address (at
least) the following issues:

- **How to parallelize the editing.** Of course, the central challenge of
    this project is to parallelize the editing process. Think about what
    can be done in parallel, and what must be done serially by a single
    thread, and design your parallel sed as appropriate.

    One interesting issue that the "best" implementations will handle is this:
    what happens if one thread runs more slowly than another? Does the
    compression give more work to faster threads? 

- **How to determine how many threads to create.** On Linux, this means using
    interfaces like `get_nprocs()` and `get_nprocs_conf()`; read the man pages
    for more details. Then, create threads to match the number of CPU
    resources available.

- **How to access the input file efficiently.** On Linux, there are many ways
    to read from a file, including C standard library calls like `fread()` and
    raw system calls like `read()`. One particularly efficient way is to use
    memory-mapped files, available via `mmap()`. By mapping the input file
    into the address space, you can then access bytes of the input file via
    pointers and do so quite efficiently. `mmap` is a super cool tool and is internally called
    in many functions you use regularly (like `malloc` and `fread`). 


## Grading

Your code should compile (and should be compiled) with the following flags:
`-Wall -Werror -pthread -O`. The last one is important: it turns on the
optimizer! In fact, for fun, try timing your code with and without `-O` and
marvel at the difference.

Your code will first be measured for correctness, ensuring that it seds input
files correctly.

If you pass the correctness tests, your code will be tested for performance;
higher performance will lead to better scores.



