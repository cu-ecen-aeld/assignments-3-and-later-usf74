#!/bin/bash

if [ $# -lt 2 ] || [ ! -d "$1" ]
then
	exit 1
else
	Xcount=$(find "$1" -type f | wc -l)
	Ycount=$(grep -r "$2" "$1" | wc -l)
	echo The number of files are $Xcount and the number of matching lines are $Ycount
fi
