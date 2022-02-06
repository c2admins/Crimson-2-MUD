#!/bin/bash

# Script to ease the porting of zones from the developer port to the production port.

# Path to the developer port areas folder (with a / at the end.)
devareas=${HOME}/c2/dev/lib/areas/

# Path to the production port areas folder (with a / at the end.)
prodareas=${HOME}/c2/prod/lib/areas/

# Directory name for prod backups.
backupfolder=automated_port_backups/

PerformPortForFile(){
	echo $1
	if [ -e ${devareas}$1 ]
		then
		if [ -e ${prodareas}$1 ]
			then
			if [ ! -d ${prodareas}${backupfolder} ]
				then
				echo "Backup folder does not exist.  Creating..."
				mkdir ${prodareas}${backupfolder}
			fi
			mv ${prodareas}$1 ${prodareas}${backupfolder}$1.bak_`date "+%b_%d_%Y_%I:%M:%S"`
		fi
		cp ${devareas}$1 ${prodareas}$1
	else
		echo "ERROR:  Cannot find file ${devareas}$1 required to port the zone!"
	fi

	return 0
}

user_continue="y"
user_confirm="n"

# Say hello....
echo "Welcome to the new Crimson II MUD zone porting script.  Allow me to verify some settings, then I'll be right with you!"
	
# Check some settings:
if [ ! -d ${devareas} ]
	then
	echo "ERROR!  It's not there.  Please either port manually or update this script!"
	exit 4500
fi
echo "Success!"
	
if [ ! -d ${prodareas} ]
	then
	echo "ERROR!  It's not there.  Please either port manually or update this script!"
	exit 4000
fi
echo "Success!"	

# Make this a loop to update more than one easier.
while [ ${user_continue} == "y" ] || [ ${user_continue} == "yes" ]; do
	# Done checking settings, let's get to it!
	# For to be pretty:
	echo; echo; echo -n "OK.  I'm back.  Let's get started.  What's the name of the Zone you'd like to port? "
	read zname

	echo "OK.  You typed ${zname}.  Checking...."
	if [ ! -e ${devareas}${zname}.wld ] && [ ! -e ${devareas}${zname}.obj ] && [ ! -e ${devareas}${zname}.mob ] && [ ! -e ${devareas}${zname}.zon ]
		then
		echo "Hmmm....  It seems like the zone you specified doesn't exist.  The name should be in the zlist command."
	else
		echo "OK, everything looks good with ${zname}."
		user_confirm="y"
	fi

	until [ ${user_confirm} == "y" ]; do
		echo -n "What's the name of the Zone you really want to port? "
		read zname
		echo -n "OK. You typed ${zname}.  Checking...."
		if [ ! -e ${devareas}${zname}.wld ] && [ ! -e ${devareas}${zname}.obj ] && [ ! -e ${devareas}${zname}.mob ] && [ ! -e ${devareas}${zname}.zon ]
			then
			echo "Hmmm....  It seems like the zone you specified doesn't exist.  The name should be in the zlist command."
		else
			echo "OK, everything looks good with ${zname}."
			user_confirm="y"
		fi
	done

	# OK, by now, we know the zone, and have verified that.  Now we just find out which files to move and port!
	echo "OK.  Now we just need what's been updated."

	echo -n "Should we port the rooms? [y/N] "
	read user_confirm
	if [ "${user_confirm}" == "y" ] || [ "${user_confirm}" == "yes" ]
		then
		echo -n "OK.  Porting rooms..."
		PerformPortForFile ${zname}.wld
		if [ $? != "0" ]
			then
			echo "ERROR:  Could not port ${zname}.wld!  Aborting!"
			exit 1
		fi
	else
		echo "OK, not porting rooms...."
	fi

	echo -n "Should we port the mobs? [y/N] "
	read user_confirm
	if [ "${user_confirm}" == "y" ] || [ "${user_confirm}" == "yes" ]
		then
		echo -n "OK.  Porting mobs..."
		PerformPortForFile ${zname}.mob
		if [ $? != "0" ]
			then
			echo "ERROR:  Could not port ${zname}.mob!  Aborting!"
			exit 1
		fi
	else
		echo "OK, not porting mobs...."
	fi

	echo -n "Should we port the objects? [y/N] "
	read user_confirm
	if [ "${user_confirm}" == "y" ] || [ "${user_confirm}" == "yes" ]
		then
		echo -n "OK.  Porting objects..."
		PerformPortForFile ${zname}.obj
		if [ $? != "0" ]
			then
			echo "ERROR:  Could not port ${zname}.obj!  Aborting!"
			exit 1
		fi
	else
		echo "OK, not porting objects...."
	fi

	echo -n "Should we port the zcmds? [y/N] "
	read user_confirm
	if [ "${user_confirm}" == "y" ] || [ "${user_confirm}" == "yes" ]
		then
		echo -n "OK.  Porting zcmds..."
		PerformPortForFile ${zname}.zon
		if [ $? != "0" ]
			then
			echo "ERROR:  Could not port ${zname}.zon!  Aborting!"
			exit 1
		fi
	else
		echo "OK, not porting zcmds...."
	fi

	echo; echo -n "OK, done porting ${zname}.  Would you like to port another Zone? (y/n) "

	read user_continue
done

echo "OK, well....  See ya next time!"

exit 0
