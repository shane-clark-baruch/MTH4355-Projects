#!/bin/bash

echo "=== Basic Commands ==="
echo "ls" | ./shell

echo "=== Pipeline ==="
echo "echo hello world | tr a-z A-Z" | ./shell

echo "=== Redirection ==="
echo 'echo hi > out.txt' | ./shell
echo 'cat < out.txt' | ./shell

echo "=== Quoted Strings ==="
echo 'echo "hello world"' | ./shell

echo "=== Background Jobs ==="
printf "sleep 3 &\njobs\n" | ./shell

echo "=== Job Control (fg/bg demo) ==="
printf "sleep 5 &\njobs\nfg %1\n" | ./shell
