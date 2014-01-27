#!/bin/bash
#
# This test makes sure the clock has been set
# Usually the clock isn't correct until the internet is available
#  also mining is usually pointless until it's probably correct
year="`date +%Y`"
if [ "$year" -lt "2014" ] ; then
 echo "Date '`date`' - clock is wrong - no point mining yet ..."
 exit 1
fi
#
nam="cgrun"
while true ; do
 echo "`date`: Starting cgminer" 2>&1 | tee -a "$HOME/$nam.log"
 #
 now="`date +%Y%m%d%H%M%S`"
 #
 ulimit -c 2097152
 #
 # Any options passed to this script will be passed to cgminer
 # via the "$@" further below
 # You ether need ~/.cgminer/cgminer.conf with your settings and pools
 # or edit the lines below to add them
 opts="--api-listen"
 ./cgminer $opts "$@" 2> "$HOME/run.$now.$$.log"
 #
 s="$?"
 # 
 echo "`date`: cgminer exited status ($s)" 2>&1 | tee -a "$HOME/$nam.log"
 #
 echo "`date`: Sleeping for 5..." 2>&1 | tee -a "$HOME/$nam.log"
 #
 sleep 5
done
