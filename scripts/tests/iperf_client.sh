#!/bin/bash

run_client(){
#    iperf3 -c 127.0.0.1 -p 22222 -t 60

    timeout 1s iperf3 -c 127.0.0.1 -p 22222 -t 60
}

while true
do
   # for i in {1..10}
  #  do
        run_client $i #& # Put a function in the background
 #   done

#    sleep 5
done



#iperf3 -s -p 33333

