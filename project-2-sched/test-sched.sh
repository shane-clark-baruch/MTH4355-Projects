#! /bin/bash

if ! [[ -x sched ]]; then
    echo "sched executable does not exist"
    exit 1
fi

../tester/run-tests.sh $*


