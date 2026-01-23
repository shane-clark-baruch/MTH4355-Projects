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
Instead, our process' will run by spinning for a short period of time.

## Program Specifications