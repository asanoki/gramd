#!/bin/sh

PID=`pgrep gramd`
if [ -z $PID ]; then
	./start.sh
fi

PORT=`cat /tmp/.gramd`
for a in $(seq 0 1 100)
do
	nc localhost $PORT < query.txt
done
