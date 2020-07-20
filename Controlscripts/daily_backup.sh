#!/bin/bash

# Automated backup script for Crimson 2 MUD.

# Written by Frank Fleschner, All Rights Reserved.
# Used with permission

# All the old backup files to rotate to be rotated.  Just add them to this list.
BackupFiles="kits.tgz players.tgz zones.tgz devzones.tgz"

# The directories to run the script in.
BackupDirectories="/home/ubuntu/c2/controlscripts/backups"

# How many backups to keep.
# MUST ME 3 OR MORE!!!
FilesToKeep=6

RotateBackupFile() {
	# Case 1 -- The normal case.  If the file is there, delete it, move the next oldest in to place.
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

	# Case 2 -- The normal case.  Move the original backup file to file.tgz.1
	if [ -e "$1.1" ]
	then
		rm $1.1
	fi
	if [ -e "$1" ]
	then
		mv $1 $1.1
	fi

	return 0
}

# Rotate older backups.
for dir in $BackupDirectories
	do
	cd $dir

	for file in $BackupFiles
		do
		RotateBackupFile $file
	done
done

# Actually backup files now.
tar -cz --file=${HOME}/c2/controlscripts/backups/kits.tgz ${HOME}/c2/prod/lib/rentfiles
tar -cz --file=${HOME}/c2/controlscripts/backups/players.tgz ${HOME}/c2/prod/lib/players
tar -cz --file=${HOME}/c2/controlscripts/backups/zones.tgz ${HOME}/c2/prod/lib/areas
tar -cz --file=${HOME}/c2/controlscripts/backups/devzones.tgz ${HOME}/c2/dev/lib/areas

exit 0
