#!/bin/sh

# put date on the filename
dt=`date +%b%d%y-%H%M`

sh createVersion.sh CG-Test-${dt}
