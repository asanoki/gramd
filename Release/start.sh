#!/bin/sh

AUTO_PORT_FILENAME=/tmp/.gramd
DICTIONARIES="/opt/home/fedora/ngrams-2.txt /opt/home/fedora/ngrams-3.txt"
OPTIONS="--interactive"

`killall gramd 2> /dev/null`
if [ $? -eq 0 ]; then
	echo "Killed active processes."
	sleep 1
fi

rm $AUTO_PORT_FILENAME 2> /dev/null
./gramd $DICTIONARIES --auto $AUTO_PORT_FILENAME $OPTIONS

