#!/bin/bash

# New Log rolling script for Crimson2 MUD.

# Written by Frank Fleschner, All rights reserved.
# Used with permission.

# All the log files to rotate to be rotated.  Just add them to this list.
LogFiles="mudrunning.txt"

# The directories to run the script in.
LogDirectories="/home/crimson/c2/controlscripts" 

# How many back logs to keep.
# MUST ME 3 OR MORE!!!
FilesToKeep=7

RotateLogFile() {
	
	# Case 1 -- The standard case.  If the file is there, delete it, move the next oldest in to place.
	for ((i=$FilesToKeep ; i > 1 ; i--))
	do
		if [ -e "$1.$i" ]
		then
			rm $1.$i
		fi
		if [ -e "$1.$((i-1))" ]
		then
			mv $1.$((i-1)) $1.$i
		fi
	done

	# Case 2 -- The special case.  Move the original log file to file.1.  Also touch the log file so it exists, so
	# the MUD doesn't freak.
	if [ -e "$1.1" ]
	then
		rm $1.1
	fi
	if [ -e "$1" ]
	then
		mv $1 $1.1
	fi
	touch $1

	return 0
}

for dir in $LogDirectories
	do
	cd $dir

	for log in $LogFiles
		do
		RotateLogFile $log
	done
done
exit 0
