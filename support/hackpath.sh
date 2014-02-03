#!/bin/sh

while test $# != 0; do
	test "$1" == "./" && { shift; continue; };
	test "$1" == . && { shift; continue; };
	test -e "$1" && printf "%s\n" $1;
	shift;
done;

