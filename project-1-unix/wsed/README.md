
In this directory, you should write the program `wsed.c` and compile it into
the binary `wsed` (e.g., `gcc -o wsed wsed.c -Wall -Werror`).

After doing so, you can run the tests from this directory by running the
`test-wsed.sh` script. If all goes well, you will see:

```sh
prompt> ./test-wsed.sh
test 1: passed
test 2: passed
test 3: passed
test 4: passed
test 5: passed
...
test 13: passed
prompt>
```

The `test-wsed.sh` script is just a wrapper for the `run-tests.sh` script in
the `tester` directory of this repository. This program has a few options; see
the relevant
[README](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/tester/README.md)
for details.
