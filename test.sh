#!/bin/bash

killall num_server
killall num_client

./num_server | sed -E 's/^/ser: /' &

(
  (
  for i in `seq 1 10` ; do
    ./num_client -s 10 test | sed -E "s/^/s$i: /" &
  done
  ) &

  (
  for i in `seq 1 10` ; do
    ./num_client -r test | sed -E "s/^/r$i: /" &
  done
  ) &

  wait
)

killall num_server
echo done
