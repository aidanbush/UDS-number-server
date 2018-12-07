#!/bin/bash

./num_server  &
pid=$!

./num_client -v -s $RANDOM socket

./num_client -v -r socket

kill -2 $pid
