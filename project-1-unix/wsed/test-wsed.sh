#! /bin/bash

if ! [[ -x wsed ]]; then
    echo "wsed executable does not exist"
    exit 1
fi

../../tester/run-tests.sh $*


