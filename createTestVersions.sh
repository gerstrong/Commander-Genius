#!/bin/sh

# put date on the filename
dt=`date +%b%d%y-%H%M`

# Windows Version
cd Win32
#make clean
#make
cp CommanderGenius.exe ../CGenius.exe
cd ..
zip CG-SVN-W32-$dt.zip ./CGenius.exe
zip CG-SVN-W32-$dt.zip ./resolutions.cfg
zip CG-SVN-W32-$dt.zip ./changelog.txt
zip CG-SVN-W32-$dt.zip ./readme.txt
rm ./CGenius.exe

# Linux 64-bit Version
cd Linux64
#make clean
#make
cp CommanderGenius ../CGenius
cd ..
zip CG-SVN-Linux64-$dt.zip ./CGenius
zip CG-SVN-Linux64-$dt.zip ./resolutions.cfg
zip CG-SVN-Linux64-$dt.zip ./changelog.txt
zip CG-SVN-Linux64-$dt.zip ./readme.txt
rm ./CGenius

# Linux 32-bit Version
cd Linux32
#make clean
#make
cp CommanderGenius ../CGenius
cd ..
zip CG-SVN-Linux32-$dt.zip ./CGenius
zip CG-SVN-Linux32-$dt.zip ./resolutions.cfg
zip CG-SVN-Linux32-$dt.zip ./changelog.txt
zip CG-SVN-Linux32-$dt.zip ./readme.txt
rm ./CGenius

# upload the packages
#rsync -e ssh CG-SVN-W32-$dt.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CG-SVN-Linux32-$dt.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CG-SVN-Linux64-$dt.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
