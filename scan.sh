#!/bin/bash
./build.sh -bue 2>&1 |
    ack -i 'scan-view' |
    sed -n "s/^.*\(\/tmp\/scan-build[^']*\)'.*/scan-view \1/p"
