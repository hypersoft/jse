#!/bin/sh

# recursive tree list
while test $# != 0; do
	test "$1" == "*" && break;
	test "$1" == . && { shift; continue; }
	test "$1" == "./" && { shift; continue; }
	test -d "$1" && printf "%s\n" "$1" && "$0" "$1"/*;
	shift;
done;


