#!/bin/sh

NAME=cgenius
VERSION=0313

# extract the data getting rid of .svn
svn export vfsroot vfsrootnonsvn
mv vfsroot vfstemp
mv vfsrootnonsvn vfsroot

# Windows Version
cd Win32
make clean
make
cp CommanderGenius.exe ../CGenius.exe
cd ..
zip -r ${NAME}-v${VERSION}-win32.zip ./vfsroot
zip ${NAME}-v${VERSION}-win32.zip ./libogg-0.dll
zip ${NAME}-v${VERSION}-win32.zip ./libogg-0.dll
zip ${NAME}-v${VERSION}-win32.zip ./libvorbis-0.dll
zip ${NAME}-v${VERSION}-win32.zip ./libvorbisenc-2.dll
zip ${NAME}-v${VERSION}-win32.zip ./libvorbisfile-3.dll
zip ${NAME}-v${VERSION}-win32.zip ./SDL.dll
zip ${NAME}-v${VERSION}-win32.zip ./CGenius.exe
zip ${NAME}-v${VERSION}-win32.zip ./changelog.txt
zip ${NAME}-v${VERSION}-win32.zip ./readme.txt
rm ./CGenius.exe

# Linux 64-bit Version
cd Linux64
make clean
make
cp CommanderGenius ../CGenius
cp CommanderGenius ../distro/Ubuntu/usr/games
cd ..
zip -r ${NAME}-v${VERSION}-linux64.zip ./vfsroot
zip ${NAME}-v${VERSION}-linux64.zip ./CGenius
zip ${NAME}-v${VERSION}-linux64.zip ./changelog.txt
zip ${NAME}-v${VERSION}-linux64.zip ./readme.txt
rm ./CGenius

# now create the 64-bit ubuntu package
cd distro 
./genubuntuamd64.sh
cd ..
mv distro/CGenius_x64.deb ${NAME}-v${VERSION}-x64.deb

# Linux 32-bit Version
cd Linux32
make clean
make
cp CommanderGenius ../CGenius
cp CommanderGenius ../distro/Ubuntu/usr/games
cd ..
zip -r ${NAME}-v${VERSION}-linux32.zip ./vfsroot
zip ${NAME}-v${VERSION}-linux32.zip ./CGenius
zip ${NAME}-v${VERSION}-linux32.zip ./changelog.txt
zip ${NAME}-v${VERSION}-linux32.zip ./readme.txt
rm ./CGenius

# now create the 32-bit ubuntu package
cd distro 
./genubuntui386.sh
cd ..
mv distro/CGenius_i386.deb ${NAME}-v${VERSION}-i386.deb

rm -r vfsroot
mv vfstemp vfsroot

# upload the packages
#rsync -e ssh ${NAME}-v${VERSION}-win32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh C${NAME}-v${VERSION}-linux32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh ${NAME}-v${VERSION}-linux64.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
