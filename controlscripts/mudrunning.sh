#!/bin/bash
case "$(pidof bin/crimsond | wc -w)" in
0)
echo "Restarting MUD all ports $(date)">>/home/ubuntu/c2/controlscripts/mudrunning.txt
# Restarts Production Port
rm /home/ubuntu/c2/controlscripts/.prodlockfile
cd /home/ubuntu/c2/controlscripts
./start_prod
# Restarts Zone Port
rm /home/ubuntu/c2/controlscripts/.devlockfile
cd /home/ubuntu/c2/controlscripts
./start_dev
;;
1) # One servers is not up
echo "Restarting down MUD port $(date)">>/home/ubuntu/c2/controlscripts/mudrunning.txt
if ! pgrep -x .autorunprod.sh > /dev/null
then
echo "  Restarting Production port $(date)">>/home/ubuntu/c2/controlscripts/mudrunning.txt
# Restarts Production Port
rm /home/ubuntu/c2/controlscripts/.prodlockfile
cd /home/ubuntu/c2/controlscripts
./start_prod
fi
if ! pgrep -x .autorundev.sh > /dev/null
then
echo "  Restarting Zone port $(date)">>/home/ubuntu/c2/controlscripts/mudrunning.txt
# Restarts Zone Port
rm /home/ubuntu/c2/controlscripts/.devlockfile
cd /home/ubuntu/c2/controlscripts
./start_dev
fi
;;
2) # All is OK
if ! pgrep -x .autorunprod.sh > /dev/null
then
echo "  Restarting Production port $(date)">>/home/ubuntu/c2/controlscripts/mudrunning.txt
# Restarts Production Port
rm /home/ubuntu/c2/controlscripts/.prodlockfile
cd /home/ubuntu/c2/controlscripts
./start_prod
fi
if ! pgrep -x .autorundev.sh > /dev/null
then
echo "  Restarting Zone port $(date)">>/home/ubuntu/c2/controlscripts/mudrunning.txt
# Restarts Zone Port
rm /home/ubuntu/c2/controlscripts/.devlockfile
cd /home/ubuntu/c2/controlscripts
./start_dev
fi
echo " Two ports up $(date)">>/home/ubuntu/c2/controlscripts/mudrunning.txt
;;
3) # Test or Hellnight Port is up
echo " Three ports are up $(date)">>/home/ubuntu/c2/controlscripts/mudrunning.txt
;;
*) echo "Something is wrong: $(date)">>/home/ubuntu/c2/controlscripts/mudrunning.txt
;;
esac
