#!/bin/sh

# Windows Version
cmake . -DBUILD_TYPE=WIN32 -DDEBUG=OFF -DOGG=yes -DTREMOR=NO -DHAVE_64_BIT=1
make package
rm CMakeFiles -R

# Linux 64-bit Version
cmake . -DBUILD_TYPE=LINUX64 -DDEBUG=OFF -DOGG=yes -DTREMOR=NO -DHAVE_64_BIT=1 -DCREATE_DEBS=1
make package
rm CMakeFiles -R

# Linux 32-bit Version
cmake . -DBUILD_TYPE=LINUX32 -DDEBUG=OFF -DOGG=yes -DTREMOR=NO -DHAVE_64_BIT=1 -DCREATE_DEBS=1
make package
rm CMakeFiles -R

# upload the packages
#rsync -e ssh CG-SVN-${dt}-win32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CCG-SVN-${dt}-linux32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CG-SVN-${dt}-linux64.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
