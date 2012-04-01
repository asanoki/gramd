#!/bin/sh

AUTO_PORT_FILENAME=/tmp/.gramd
DICTIONARIES="../data/ngrams-2-big.txt"
BIN_COMMAND=gramd # <- Change it if installed in custom location

OPTIONS="--interactive --verbose --epsilon 0.0001"
#OPTIONS="--verbose --epsilon 0.001"

`killall gramd 2> /dev/null`
if [ $? -eq 0 ]; then
	echo "Killed active processes."
	sleep 1
fi

rm $AUTO_PORT_FILENAME 2> /dev/null
$BIN_COMMAND $DICTIONARIES --auto $AUTO_PORT_FILENAME $OPTIONS
