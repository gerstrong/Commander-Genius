#!/bin/sh

# put date on the filename
dt=`date +%b%d%y-%H%M`

# extract the data getting rid of .svn
svn export vfsroot vfsrootnonsvn

# Windows Version
cd Win32
cp CommanderGenius.exe ../CGenius.exe
cd ..

cd vfsrootnonsvn
zip -r ../CG-SVN-${dt}-win32.zip ./
cd ..
zip CG-SVN-${dt}-win32.zip ./libogg-0.dll
zip CG-SVN-${dt}-win32.zip ./libogg-0.dll
zip CG-SVN-${dt}-win32.zip ./libvorbis-0.dll
zip CG-SVN-${dt}-win32.zip ./libvorbisenc-2.dll
zip CG-SVN-${dt}-win32.zip ./libvorbisfile-3.dll
zip CG-SVN-${dt}-win32.zip ./SDL.dll
zip CG-SVN-${dt}-win32.zip ./CGenius.exe
zip CG-SVN-${dt}-win32.zip ./changelog.txt
zip CG-SVN-${dt}-win32.zip ./readme.txt
rm ./CGenius.exe

# Linux 64-bit Version
cd Linux64
cp CommanderGenius ../CGenius
cp CommanderGenius ../distro/Ubuntu/usr/games
cd ..

cd vfsrootnonsvn
zip -r ../CG-SVN-${dt}-linux64.zip ./
cd ..
zip CG-SVN-${dt}-linux64.zip ./CGenius
zip CG-SVN-${dt}-linux64.zip ./changelog.txt
zip CG-SVN-${dt}-linux64.zip ./readme.txt
rm ./CGenius

# now create the 64-bit ubuntu package
cd distro 
./genubuntuamd64.sh
cd ..
mv distro/CGenius_x64.deb CG-SVN-${dt}-x64.deb

# Linux 32-bit Version
cd Linux32
cp CommanderGenius ../CGenius
cp CommanderGenius ../distro/Ubuntu/usr/games

cd ..
cd vfsrootnonsvn
zip -r ../CG-SVN-${dt}-linux32.zip ./
cd ..
zip CG-SVN-${dt}-linux32.zip ./CGenius
zip CG-SVN-${dt}-linux32.zip ./changelog.txt
zip CG-SVN-${dt}-linux32.zip ./readme.txt
rm ./CGenius

# now create the 32-bit ubuntu package
cd distro 
./genubuntui386.sh
cd ..
mv distro/CGenius_i386.deb CG-SVN-${dt}-i386.deb

rm -r vfsrootnonsvn

# upload the packages
#rsync -e ssh CG-SVN-${dt}-win32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CCG-SVN-${dt}-linux32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh CG-SVN-${dt}-linux64.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
