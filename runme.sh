#!/bin/bash
make build

pids=()
for (( i=0; i<10; i++ ))
do
	./main test &
	pids+=($!)
done

sleep 5m

for pid in ${pids[@]}
do
	kill -SIGINT $pid
done

rm test.lck
