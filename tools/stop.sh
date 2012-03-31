#!/bin/sh

`killall gramd 2> /dev/null`
if [ $? -eq 0 ]; then
	echo "Killed active processes."
	exit
fi

echo "Nothing to stop."
