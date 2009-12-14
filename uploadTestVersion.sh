#!/bin/sh

# put date on the filename
dt=`date +%b%d%y-%H%M`

# Windows Version
cmake -DBUILD_TYPE=WIN32 -D64BIT=ON
make

zip CG-SVN-W32-$dt.zip ./build/Windows/CGenius.exe
zip CG-SVN-W32-$dt.zip ./data/resolutions.cfg
zip CG-SVN-W32-$dt.zip ./changelog.txt
zip CG-SVN-W32-$dt.zip ./readme.txt
make clean

# Linux 64-bit Version
#cmake -DBUILD_TYPE=LINUX64 -D64BIT=ON
#make

#zip CG-SVN-Linux64-$dt.zip ./build/Linux64/CGenius
#zip CG-SVN-Linux64-$dt.zip ./data/resolutions.cfg
#zip CG-SVN-Linux64-$dt.zip ./changelog.txt
#zip CG-SVN-Linux64-$dt.zip ./readme.txt
#make clean

# Linux 32-bit Version
#cmake -DBUILD_TYPE=LINUX32 -D64BIT=ON
#make

#zip CG-SVN-Linux32-$dt.zip ./build/Linux32/CGenius
#zip CG-SVN-Linux32-$dt.zip ./data/resolutions.cfg
#zip CG-SVN-Linux32-$dt.zip ./changelog.txt
#zip CG-SVN-Linux32-$dt.zip ./readme.txt
#make clean

# upload the packages
rsync -e ssh CG-SVN-W32-$dt.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CG-SVN-Linux32-$dt.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CG-SVN-Linux64-$dt.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
cd CMakeFiles
rm -R *
cd ..
cd Debug 
rm -R *
