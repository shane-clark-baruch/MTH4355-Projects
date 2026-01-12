
# Projects for an Operating Systems Class

This repository holds the projects for MTH 4355: Operating Systems at CUNY Baruch. 
They are based on the OSTEP textbook and built upon the projects
at the University of Wisconsin-Madison.

We are developing some tests to see if your code works. The testing framework that is
currently available is found [here](https://github.com/shane-clark-baruch/MTH4355-Projects/tree/master/tester).
A specific testing script, found in each project directory, can be used to run
the tests against your code. 

For example, in the initial utilities project, the relatively simple `wcat`
program that you create can be tested by running the `test-wcat.sh` script.
This could be accomplished by the following commands:

```sh
prompt> git clone https://github.com/remzi-arpacidusseau/ostep-projects
prompt> cd ostep-projects/initial-utilities/wcat
prompt> gcc -o wcat wcat.c -Wall 
prompt> ./test-wcat.sh
test 1: passed
test 2: passed
test 3: passed
test 4: passed
test 5: passed
test 6: passed
test 7: passed
prompt> 
```
Of course, this sequence assumes your (a)
code is written in one shot (impressive!), and (b) that it works perfectly
(well done!). Even for simple assignments, it is likely that the
compile/run/debug cycle might take a few iterations.

## C/Linux Projects

### Project 1: Unix Utilities

This project is meant to get you warmed up with programming in the C/UNIX
environment. None are meant to be particularly hard, but should be enough so
that you can get more comfortable programming. 

Realize the best thing you can do to learn to program in any environment is to
program **a lot**. These small projects are only the beginning of that
journey; you'll have to do more on your own to truly become proficient.

* [Project 1: Unix Utilities](project-1-unix) (cat, grep, sed)

### Project 2: Schedulers (Unfinished)

This project has you implement variouis schedulers we learned in class as a standalone program
Furthermore, you will be asked to compute some of the metrics from class (turnaround time, response time, etc).

* Project 2: Scheduler


### Processes and Scheduling 

* [Project 3: Shell](project-3-shell)


### Concurrency (Unfinished)

* [Project 4: Concurrency](project-4-concurrency)

<!-- 
### File Systems

* [File System Checker](filesystems-checker)

### Distributed Systems


 ( ## Kernel Hacking Projects (xv6)

These projects all are to be done inside the
[xv6](https://pdos.csail.mit.edu/6.828/2017/xv6.html) kernel based on an early
version of Unix and developed at MIT. Unlike the C/Linux projects, these give
you direct experience inside a real, working operating system (albeit a simple
one).

Read the [install notes](INSTALL-xv6.md) to see how to download the latest xv6 
and install the tools you'll need.

### Initial Projects

* [Intro To xv6](initial-xv6)

### Processes and Scheduling

* [Scheduling (Lottery)](scheduling-xv6-lottery)

### Virtual Memory

* [Virtual Memory (Null Pointer and Read-Only Regions)](vm-xv6-intro)

### Concurrency

* [Kernel Threads (Basic Implementation)](concurrency-xv6-threads)

### File Systems)
>



