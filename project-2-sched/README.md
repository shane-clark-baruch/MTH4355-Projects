# Scheduler

In this project, you'll build a simple process structure and implement various schedulers. 
The scheduler is the orchestrator of the operating system and plays a key role in the efficiency
of your system.
Our goal is to have you build multiple schedulers to familiarize ourselves with their construction, strengths, and weaknesses.

There are three specific objectives to this assignment:

* To further familiarize yourself with the process structure.
* To learn how processes are scheduled and their states are managed.


## Overview

In this assignment, you will implement a *scheduler (sched)*. The shell should operate in this
basic way: you will pass this scheduler a scheduling policy and a sequence of processes *(name, arrival time, run-time, I/O)*


The scheduler you implement will be similar to, but simpler than, the one you run
every day in Unix. However, we will not be allocating memory for these process and performing context switches. 
Instead, our process' will "run" by printing out a message. 


## Program Specifications

The call of your scheduler will always fit the format:

```.sh
> ./sched -m [mode] < [process file]
```

* `mode`: Specifies the scheduling policy your scheduler will create. Accepted options are `FIFO, SJF, STCF,` and `RR` to match the policies we covered in class. For simplicity, **ALL** schedulers will be I/O aware. Meaning, if a process requests an I/O, then that process becomes blocked and another process can be scheduled.
* `[process file]`: This file will contain a list of processes that you will schedule. Each process will be specified with a comma separated list `arrival,run-length,IO-time,IOlength`. All components must be specified by a non-negative integer. 
  * `arrival`- The time which the process becomes *Ready* for the first time. A process may be scheduled at the first time unit it arrives.
  * `run-length`- The total CPU time required to complete the process. Note that this does not include the time that a process spends *Blocked* while waiting for an IO to complete.
  * `IO-time`- The moment a progress will initiate an IO request. Note that this depends on the amount of progress a program has made. 
  * `IO-length`- The time required for a process to stay in the *Blocked* state.

For example, lets assume we have a process file `input.txt`: 

```.c
0,5,2,1
1,3,0,0
```

Creates two processes, `A` and `B`.  Process A arrives at time 1 and issues an IO after running for 2 time units, becomes blocked for 1 time unit, and will then run for an additional 4 units. Process B arrives at time 1 does not issue an IO time and finishes after running for three time units.

All proecsses should be named in alphabetical order starting with A. 

## Output 

The output of your scheduler should be a state table for your processes, as well as, the *Average Turn-around Time* and *Average Response Time*. This table should include 4 states, `Running, Runnable, Blocked, Dead`. If a process has not yet arrived, then there should be nothing printed out in that corresponding state table. 

For example, using the input files outlined above and using the FIFO policy, we obtain the following state table: 

```.sh
> ./sched -m FIFO < input.txt
t  | Process A | Process B |
0  | Running   |           |
1  | Running   | Runnable  |
2  | Blocked   | Running   |
3  | Runnable  | Running   |
4  | Runnable  | Running   |
5  | Running   | Dead      |
6  | Running   | Dead      |
7  | Running   | Dead      |
Average Turn-around Time: 6.000
Average Response Time: 3.000
```



**Details**
* **sched** must be invoked with a mode and process file. If no mode is detected, **sched** assumes FIFO policy.
* If any processes do not fit the format above, your program should print out `Invalid process detected` (followed by a new line) and exit with status 1.
* In all non-error cases, **sched** should exit with status code 0, usually by
  returning a 0 from **main()** (or by calling **exit(0)**).
*  **sched** can only accept one mode at a time. If the user attempts to invoke both, then `sched` should print
  `sched -m [mode] < [process file]` (followed by a newline) and exit with
  status 1.  


## Useful C stuff
In addition to the functions covered in class so far, you will also need: 

* `strtok`, `strtok_r` and `strsep`: Check out the man pages and see why/how you will need them! One may be more fit to your needs than the others, but you should check each of them! 
* Structures will help you keep track of your processes info. One may include a name, arrival time, run time, io time, io length... and some others in their process structure.
* Make a modular program! You are going to have 4 different scheduling policies. I would recommend having a function for each of them. 






