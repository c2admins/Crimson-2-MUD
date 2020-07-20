#! /bin/bash
#  Autorun script for c2.  Culled nearly exclusively from the autorun.sh script that came with
#  CircleMUD 3.1 :
#  Contributions by Fred Merkel, Stuart Lamble, and Jeremy Elson
#  Copyright (c) 1996 The Trustees of The Johns Hopkins University

#  Modified by Relic to do what c2 wants it to, how c2 wants to do it.
#  Further extensions will also be made to the code to support it.

#  May 2004, modified by Relic.  Lockfiles, email alerts and rolling core files that don't have
#  the PID attached to the name.

#  August 2007, modified by Relic.  Changing some structure.  Made all executables
#  into crimson2d to follow a more common unix-like naming tradition.

#  Port to run on:
#  Generally, it's 4000 for the player port and 4500 for the dev port. It is also 6666 for Hellnight port
PORT=6666

#  Flags to run:
#  List of flags:
#  DISCLAIMER:  I have NO idea how well these are supported anymore
#  -?  Help (listing of the flags)
#  -d  Directory to use (followed by the directory, ie -d lib)
#  -e  create max eqp file (???)
#  -E  reset everyone's eq to database stats
#  -r  rebuild routines (???)
#  -s  no specials (no special procs, much of the hardcoding for objs, rooms, and mobs)
#  -v  verbose
#  -V  MAXIMUM verbose
#  -w  suppress weather messages
#  -z  don't reset zones
########################
FLAGS= 
########################

#  crimsond for all.
PROG=crimsond

#  File which is the lock file, so that we don't have more than one instance
#  of the script running at once.
LOCKFILE=~/c2/controlscripts/.helllockfile

#  Where we log info
LOGFILE=logs/syslog

#  What port this is, should be the name of the directory it's in.
#  Valid choices are prod or dev or hell
PORTHOMEDIR=~/c2/hell
PORTNAME=hell

#  Email address alerts go to.
MAINTAINER=c2admins@gmail.com

#  Subroutines to use when we get some signals

function terminate()
{
	# The MUD will get it's own SIGTERM, don't worry about it!
	# Every man for himself!
	echo "Caught SIGTERM, machine shutting down!" >> ${LOGFILE}
	
	rm ${LOCKFILE}
	exit
}

function hangup()
{
    # Let's ignore HUPs
    echo "Caugh SIGHUP, ignoring!" >> ${LOGFILE}
}

#  Need to do some checking here, like whether PORTHOMEDIR exists...

if [ ! -d ${PORTHOMEDIR} ]; then
	echo "This script cannot find the home directory for the port."
	echo "I'm bailing!"
	exit
fi

#  Do some stuff before we enter the loop

cd ${PORTHOMEDIR}
ulimit -c unlimited
MALLOC_CHECK_=0 

#  Trap signals:
trap terminate SIGTERM
trap hangup SIGHUP

#  Keep more than one instance from being started.
if [ -w ${LOCKFILE} ]; then
	echo "There is already an instance of this script running."
	echo ""
	echo "Unpause it by removing ~/hell/pause (if necessary) and wait for"
	echo "it to simply wake up."
	exit
fi

touch ${LOCKFILE}


###################
#### MAIN LOOP ####
###################


while ( : ) do

  DATE=`date`
  echo "autorun starting game $DATE" >> ${LOGFILE}
  echo "running bin/${PROG} ${FLAGS} ${PORT}" >> ${LOGFILE}

# LD_PRELOAD=/home/ubuntu/c2/DieHard/src/libdiehard.so
 bin/${PROG} ${FLAGS} ${PORT} >> ${LOGFILE} 2>&1

  #  Everything after this happens after the binary exits
  #  (whether that be scheduled or because of a crash)

	# Move core files.  (This is a little machine specific)
	if [ -e lib/core ]; then
		if [ ! -d cores/ ]; then
			mkdir cores
		fi
		echo "C2 autorun${PORTNAME} has detected a dropped core file in association with a return" > .letter
		echo "from the mud binary.  This most likely indicates a crash of the ${PORTNAME} port." >> .letter
		mv lib/core cores/core_`date "+%b_%d_%Y_%I:%M:%S"`
	else
	echo "C2 autorun${PORTNAME} has detected a return from the ${PORTNAME} port." > .letter
	echo "There is no core file, so no reason to believe it was a crash." >> .letter
	fi

	echo "The time is ${DATE}." >> .letter

  if [ -e .killscript ]; then
    echo "autoscript killed ${DATE}" >> ${LOGFILE}
    	rm ${LOCKFILE}
    rm .killscript
    	echo "The autorun script has been halted, please restart it!" >> .letter
    	mail -s "C2 automail" ${MAINTAINER} < .letter
    	rm .letter
    exit
  fi

  if [ ! -e .fastboot ]; then
    sleep 30
  else
    rm .fastboot
    sleep 5
  fi

  while [ -e pause ]; do
    sleep 5
 		if [ -e .letter ]; then
			echo "The autorun script has been paused, please remove ${PORTHOMEDIR}/.pause for it to resume." >> .letter
			mail -s "C2 automail" ${MAINTAINER} < .letter
			rm .letter
		fi
	done

	if [ -e .letter ]; then
		echo "This script will now attempt to bring it back up!" >> .letter
		mail -s "C2 automail" ${MAINTAINER} < .letter
		rm .letter
	fi

done
