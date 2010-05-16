#!/bin/sh

NAME=cgenius
VERSION=0312

svn export ./ svnexport
cd svnexport

# Windows Version
cd Win32
#make clean
#make
cp CommanderGenius.exe ../CGenius.exe
cd ..
zip -r ${NAME}-v${VERSION}-win32.zip ./data
zip -r ${NAME}-v${VERSION}-win32.zip ./games
zip ${NAME}-v${VERSION}-win32.zip ./libogg-0.dll
zip ${NAME}-v${VERSION}-win32.zip ./libvorbis-0.dll
zip ${NAME}-v${VERSION}-win32.zip ./libvorbisenc-2.dll
zip ${NAME}-v${VERSION}-win32.zip ./libvorbisfile-3.dll
zip ${NAME}-v${VERSION}-win32.zip ./SDL.dll
zip ${NAME}-v${VERSION}-win32.zip ./CGenius.exe
zip ${NAME}-v${VERSION}-win32.zip ./resolutions.cfg
zip ${NAME}-v${VERSION}-win32.zip ./changelog.txt
zip ${NAME}-v${VERSION}-win32.zip ./readme.txt
rm ./CGenius.exe

# Linux 64-bit Version
cd Linux64
#make clean
#make
cp CommanderGenius ../CGenius
cd ..
zip -r ${NAME}-v${VERSION}-linux64.zip ./data
zip -r ${NAME}-v${VERSION}-linux64.zip ./games
zip ${NAME}-v${VERSION}-linux64.zip ./CGenius
zip ${NAME}-v${VERSION}-linux64.zip ./resolutions.cfg
zip ${NAME}-v${VERSION}-linux64.zip ./changelog.txt
zip ${NAME}-v${VERSION}-linux64.zip ./readme.txt
rm ./CGenius

# here, we create the ubuntu package for 64-bit
cp CommanderGenius ../distro/Ubuntu/usr/games

# Linux 32-bit Version
cd Linux32
#make clean
#make
cp CommanderGenius ../CGenius
cd ..
zip -r ${NAME}-v${VERSION}-linux32.zip ./data
zip -r ${NAME}-v${VERSION}-linux32.zip ./games
zip ${NAME}-v${VERSION}-linux32.zip ./CGenius
zip ${NAME}-v${VERSION}-linux32.zip ./resolutions.cfg
zip ${NAME}-v${VERSION}-linux32.zip ./changelog.txt
zip ${NAME}-v${VERSION}-linux32.zip ./readme.txt
rm ./CGenius

rm -r svnexport

# upload the packages
#rsync -e ssh ${NAME}-v${VERSION}-win32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh C${NAME}-v${VERSION}-linux32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
#rsync -e ssh ${NAME}-v${VERSION}-linux64.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
