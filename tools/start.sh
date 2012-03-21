#!/bin/sh

AUTO_PORT_FILENAME=/tmp/.gramd
DICTIONARIES="../data/ngrams-2.txt ../data/ngrams-3.txt"
#OPTIONS="--interactive"

`killall gramd 2> /dev/null`
if [ $? -eq 0 ]; then
	echo "Killed active processes."
	sleep 1
fi

rm $AUTO_PORT_FILENAME 2> /dev/null
gramd $DICTIONARIES --auto $AUTO_PORT_FILENAME $OPTIONS
