#!/bin/sh

# Windows Version
cmake . -DBUILD_TARGET=WIN32 -DCMAKE_BUILD_TYPE=Release -DOGG=yes -DTREMOR=NO 
make package -j 4
rm CMakeFiles -R

# Linux 64-bit Version
cmake . -DDBFUSION=yes -DBUILD_TARGET=LINUX -DBUILD_ARCH=amd64 -DCMAKE_BUILD_TYPE=Release -DOGG=yes -DTREMOR=NO -DUSE_SDL2=yes -DCREATE_DEBS=1 -DCREATE_RPMS=1
#make package_source
make package -j 4
rm CMakeFiles -R

# Linux 32-bit Version
#cmake . -DBUILD_TARGET=LINUX -DBUILD_ARCH=i386 -DCMAKE_BUILD_TYPE=Release -DOGG=yes -DTREMOR=NO -DCREATE_DEBS=1 -DCREATE_RPMS=1
#make package -j 4
#rm CMakeFiles -R

#clean up the packaged files
rm _CPack_Packages -R

# upload the packages
#rsync -e ssh CG-SVN-${dt}-win32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CCG-SVN-${dt}-linux32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CG-SVN-${dt}-linux64.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
