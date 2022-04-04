#!/bin/bash

while :
do
    iperf3 -c 127.0.0.1 -p 22222 -t 3
#    timeout 1s iperf3 -c 127.0.0.1 -p 22222 -t 60

    if [ $? -ne 0 ]
    then
        break
    fi

done
