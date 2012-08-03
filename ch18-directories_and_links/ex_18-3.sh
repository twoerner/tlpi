#!/bin/bash

ORIGFILE=00fileorig
LINKFILE=00filelink
STARTDIR=`basename $PWD`
FIELD=`pwd | tr '/' ' ' | wc -w`
PREVDIR=`pwd | cut -d'/' -f${FIELD}`
ETCLINK=lnk
BADLINK=this

stat $ORIGFILE > /dev/null 2>&1
if [ $? -eq 0 ]; then
	echo "file '$ORIGFILE' already exists"
	exit 1
fi
stat $LINKFILE > /dev/null 2>&1
if [ $? -eq 0 ]; then
	echo "file '$LINKFILE' already exists"
	exit 1
fi
stat $ETCLINK > /dev/null 2>&1
if [ $? -eq 0 ]; then
	echo "file '$ETCLINK' already exists"
	exit 1
fi
stat $BADLINK > /dev/null 2>&1
if [ $? -eq 0 ]; then
	echo "file '$BADLINK' already exists"
	exit 1
fi

cleanup() {
	rm -f $ORIGFILE $LINKFILE $ETCLINK $BADLINK
	exit 1
}

trap cleanup EXIT
touch $ORIGFILE
ln -s $ORIGFILE $LINKFILE
ln -s /etc $ETCLINK
ln -s /here/it/is/not $BADLINK

clear
valgrind ./ex_18-3 $HOME
read

clear
valgrind ./ex_18-3 $ORIGFILE
read

clear
valgrind ./ex_18-3 $LINKFILE
read

clear
valgrind ./ex_18-3 ././..///..//././//${PREVDIR}///./././/../${PREVDIR}//${STARTDIR}/////////////
read

clear
valgrind ./ex_18-3 /usr/share/../local
read

clear
valgrind ./ex_18-3 "/usr/sh are/../local"
read

clear
valgrind ./ex_18-3 /../../..${HOME}/../../../../etc
read

clear
valgrind ./ex_18-3 ${ETCLINK}/rc.d
read

clear
valgrind ./ex_18-3 $BADLINK

cleanup
