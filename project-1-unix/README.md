
# Unix Utilities

**Before beginning:** Read this [lab tutorial](http://pages.cs.wisc.edu/~remzi/OSTEP/lab-tutorial.pdf); it has some useful tips for programming in the C environment.

In this project, you'll build a few different UNIX utilities, simple versions
of commonly used commands like **cat**, **ls**, etc. We'll call each of them a
slightly different name to avoid confusion; for example, instead of **cat**,
you'll be implementing **wcat** (i.e., "wisconsin" cat).

Objectives:
* Familiarize yourself with the C programming language
* Familiarize yourself with a shell / terminal / command-line of UNIX
* Learn a little about how UNIX utilities are implemented

Summary of what gets turned in:
* A bunch of single .c files for each of the utilities below: **wcat.c**,
  **wgrep.c**, **wzip.c**, and **wunzip.c**.
* Each should compile successfully when compiled with the **-Wall** and
**-Werror** flags.
* Each should (hopefully) pass the tests we supply to you.

## wcat

The program **wcat** is a simple program. Generally, it reads a file as
specified by the user and prints its contents. A typical usage is as follows,
in which the user wants to see the contents of main.c, and thus types: 

```
prompt> ./wcat main.c
#include <stdio.h>
...
```

As shown, **wcat** reads the file **main.c** and prints out its contents. 
The "**./**" before the **wcat** above is a UNIX thing; it just tells the
system which directory to find **wcat** in (in this case, in the "." (dot)
directory, which means the current working directory). 

To create the **wcat** binary, you'll be creating a single source file,
**wcat.c**, and writing a little C code to implement this simplified version
of **cat**. To compile this program, you will do the following:

```
prompt> gcc -o wcat wcat.c -Wall -Werror
prompt> 
```

This will make a single *executable binary* called **wcat** which you can
then run as above. 

You'll need to learn how to use a few library routines from the C standard
library (often called **libc**) to implement the source code for this program,
which we'll assume is in a file called **wcat.c**. All C code is
automatically linked with the C library, which is full of useful functions you
can call to implement your program. Learn more about the C library
[here](https://en.wikipedia.org/wiki/C_standard_library) and perhaps
[here](https://www-s.acm.illinois.edu/webmonkeys/book/c_guide/)<sup>[1](#myfootnote1)</sup>.  

For this project, we recommend using the following routines to do file input
and output: **fopen()**, **fgets()**, and **fclose()**. Whenever you use a new
function like this, the first thing you should do is read about it -- how else
will you learn to use it properly?

On UNIX systems, the best way to read about such functions is to use what are
called the **man** pages (short for **manual**). In our HTML/web-driven world,
the man pages feel a bit antiquated, but they are useful and informative and
generally quite easy to use. The Linux man pages are slightly different than those built
in on MAC, I typically use the pages recommended [here](https://man7.org/linux/man-pages/index.html).

To access the man page for **fopen()**, for example, just type the following
at your UNIX shell prompt: 
```
prompt> man fopen
```

Then, read! Reading man pages effectively takes practice; why not start
learning now?

We will also give a simple overview here. The **fopen()** function "opens" a
file, which is a common way in UNIX systems to begin the process of file
access. In this case, opening a file just gives you back a pointer to a
structure of type **FILE**, which can then be passed to other routines to
read, write, etc. 

Here is a typical usage of **fopen()**:

```c
FILE *fp = fopen("main.c", "r");
if (fp == NULL) {
    printf("cannot open file\n");
    exit(1);
}
```

A couple of points here. First, note that **fopen()** takes two arguments: the
*name* of the file and the *mode*. The latter just indicates what we plan to
do with the file. In this case, because we wish to read the file, we pass "r"
as the second argument. Read the man pages to see what other options are
available. 

Second, note the *critical* checking of whether the **fopen()** actually
succeeded. This is not Java where an exception will be thrown when things goes
wrong; rather, it is C, and it is expected (in good programs, i.e., the
only kind you'd want to write) that you always will check if the call
succeeded. Reading the man page tells you the details of what is returned when
an error is encountered; in this case, the macOS man page says:

```
Upon successful completion fopen(), fdopen(), freopen() and fmemopen() return
a FILE pointer.  Otherwise, NULL is returned and the global variable errno is
set to indicate the error. 
```

Thus, as the code above does, please check that **fopen()** does not return
NULL before trying to use the FILE pointer it returns.

Third, note that when the error case occurs, the program prints a message and
then exits with error status of 1. In UNIX systems, it is traditional to
return 0 upon success, and non-zero upon failure. Here, we will use 1 to
indicate failure.

Side note: if **fopen()** does fail, there are many reasons possible as to
why.  You can use the functions **perror()** or **strerror()** to print out
more about *why* the error occurred; learn about those on your own (using
... you guessed it ... the man pages!).

Once a file is open, there are many different ways to read from it. The one
we're suggesting here to you is **fgets()**, which is used to get input from
files, one line at a time. 

To print out file contents, just use **printf()**. For example, after reading
in a line with **fgets()** into a variable **buffer**, you can just print out
the buffer as follows:

```c
printf("%s", buffer);
```

Note that you should *not* add a newline (\\n) character to the printf(),
because that would be changing the output of the file to have extra
newlines. Just print the exact contents of the read-in buffer (which, of
course, many include a newline).

Finally, when you are done reading and printing, use **fclose()** to close the
file (thus indicating you no longer need to read from it).

**Details**

* Your program **wcat** can be invoked with one or more files on the command
  line; it should just print out each file in turn. 
* In all non-error cases, **wcat** should exit with status code 0, usually by
  returning a 0 from **main()** (or by calling **exit(0)**).
* If *no files* are specified on the command line, **wcat** should just exit
  and return 0. Note that this is slightly different than the behavior of 
  normal UNIX **cat** (if you'd like to, figure out the difference).
* If the program tries to **fopen()** a file and fails, it should print the
  exact message "wcat: cannot open file" (followed by a newline) and exit
  with status code 1.  If multiple files are specified on the command line,
  the files should be printed out in order until the end of the file list is
  reached or an error opening a file is reached (at which point the error
  message is printed and **wcat** exits). 


## wgrep

The second utility you will build is called **wgrep**, a variant of the UNIX
tool **grep**. This tool looks through a file, line by line, trying to find a
user-specified search term in the line. If a line has the word within it, the
line is printed out, otherwise it is not. 

Here is how a user would look for the term **foo** in the file **bar.txt**:

```
prompt> ./wgrep foo bar.txt
this line has foo in it
so does this foolish line; do you see where?
even this line, which has barfood in it, will be printed.
```

**Details**

* Your program **wgrep** is always passed a search term and zero or
  more files to grep through (thus, more than one is possible). It should go
  through each line and see if the search term is in it; if so, the line
  should be printed, and if not, the line should be skipped.
* The matching is case sensitive. Thus, if searching for **foo**, lines
  with **Foo** will *not* match.
* Lines can be arbitrarily long (that is, you may see many many characters
  before you encounter a newline character, \\n). **wgrep** should work
  as expected even with very long lines. For this, you might want to look
  into the **getline()** library call (instead of **fgets()**), or roll your
  own. 
* If **wgrep** is passed no command-line arguments, it should print
  "wgrep: searchterm [file ...]" (followed by a newline) and exit with
  status 1.  
* If **wgrep** encounters a file that it cannot open, it should print
  "wgrep: cannot open file" (followed by a newline) and exit with status 1. 
* In all other cases, **wgrep** should exit with return code 0.
* If a search term, but no file, is specified, **wgrep** should work,
  but instead of reading from a file, **wgrep** should read from
  *standard input*. Doing so is easy, because the file stream **stdin**
  is already open; you can use **fgets()** (or similar routines) to
  read from it.
* For simplicity, if passed the empty string as a search string, **wgrep**
  can either match NO lines or match ALL lines, both are acceptable.




## wsed

The next tool you will build is `wsed`, which is based the stream editor `sed`. This unix function has 
a wide range of utilities, but we will focus on two features, string substitution and character replacemnt.

Before we begin, I recommend you read paruse the man pages and try to use the `sed` function if you have not already. 
Let's assume that a text file called `test.txt` contains the contents

```.txt
super duper
hello there
```

then we can replace all `p`'s with `s`'s through the command.

```.sh
> sed 'y/p/s/' test.txt
suser duser
hello there
```

We can also replaces phrases with string subsitution. The example below replaces all occurences of the word `there` with `stranger`.

```.sh
sed 's/there/stranger/' test.txt
super duper
hello stranger
```
Notice that these two commands output the standard output to `stdout` by default. We will learn how to implement redirection in project 3.

**Details**

* Your program **wsed** must be invoked with an \[option\] and one file on the command
  line; it should just print out each file in turn. The options are outlined below. 
  * `./wsed s/regexp/replacement/ file`:   Attempt to match `regexp` against the pattern space. If successful, replace that portion matched with `replacement`. 
  * `./wsed y/source/dest/ file`: Transliterate the characters in the pattern space which appear in `source` to the corresponding character in `dest`. If the `source` and `dest` strings are of different lengths, then you should print `wsed: y/source/dest/ source and dest must have same length`, return 1, and exit.
* In all non-error cases, **wsed** should exit with status code 0, usually by
  returning a 0 from **main()** (or by calling **exit(0)**).
* If **wsed** is passed no command-line arguments, it should print
  "wsed: \[command\] [file]" (followed by a newline) and exit with
  status 1.  
* If the program tries to **fopen()** a file and fails, it should print the
  exact message "wsed: cannot open file" (followed by a newline) and exit
  with status code 1.  If multiple files are specified on the command line,
  the files should be printed out in order until the end of the file list is
  reached or an error opening a file is reached (at which point the error
  message is printed and **wsed** exits).

### Useful Functions

* `printf` and `fprintf`: Need these to print, check man pages for when to use which.
* `fopen` and `fclose`: Need these to open and close files.
* `getline`, `fgets` and `fgetc`: Useful functions to read lines, strings, charcaters from a file pointer.
* `strstr`, `strlen`, `strcat`, `strtok`, `strsep`: Useful functions when working with string.
* `free`: While you may not use `alloc` in your function directly, some functions mentioned above do! Make sure to read
   the man pages and determine when a `free` is needed.

### Footnotes


<a name="myfootnote1">1</a>: Unfortunately, there is a lot to learn about the
C library, but at some point, you've just got to **read documentation** to
learn what is available. Why not now, when you are young? Or, if you are old,
why not now, before it's ... ahem ... too late?






