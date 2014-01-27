#!/bin/bash
#
semfile="/tmp/cgminer-cron"
#
# Set this to wherever your cgminer.sh run script is
cmddir="/opt/cgminer/"
#
touch "$semfile"
cd "$cmddir"
sem "$semfile" "${cmddir}cgminer.sh"
