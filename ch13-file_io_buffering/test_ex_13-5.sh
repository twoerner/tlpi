#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: $0 <text file>"
	exit 1
fi

if [ -d $1 -o ! -r $1 ]; then
	echo "can't open '$1' for reading"
	exit 1
fi

TOTALLINES=`cat $1 | wc -l`
CURRENT=1

while [ 1 ]; do
	echo "testing at $CURRENT"

	TMP1=`mktemp -p .`
	TMP2=`mktemp -p .`
	tail -n $CURRENT $1 > $TMP1
	./ex_13-5 -n $CURRENT $1 > $TMP2
	diff $TMP1 $TMP2 > /dev/null
	rm -f $TMP1 $TMP2
	if [ $? -ne 0 ]; then
		echo "fails at $CURRENT"
		exit 1
	fi

	CURRENT=`expr $CURRENT + 1`
	if [ $CURRENT -gt $TOTALLINES ]; then
		break
	fi
done

echo "Success!"
exit 0
