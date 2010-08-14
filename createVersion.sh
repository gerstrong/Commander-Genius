#!/bin/sh

# Windows Version
cp build/Win32/CGenius.exe ./CGenius.exe

cd vfsroot
git archive -o ../$1-win32.zip HEAD
cd ..

zip $1-win32.zip ./libogg-0.dll
zip $1-win32.zip ./libogg-0.dll
zip $1-win32.zip ./libvorbis-0.dll
zip $1-win32.zip ./libvorbisenc-2.dll
zip $1-win32.zip ./libvorbisfile-3.dll
zip $1-win32.zip ./SDL.dll
zip $1-win32.zip ./CGenius.exe
zip $1-win32.zip ./changelog.txt
zip $1-win32.zip ./readme.txt
rm ./CGenius.exe

# Linux 64-bit Version
cp build/Linux64/CGenius ./CGenius
cp build/Linux64/CGenius ./distro/Ubuntu/usr/games

cd vfsroot
git archive -o ../$1-linux64.zip HEAD
cd ..

zip $1-linux64.zip ./CGenius
zip $1-linux64.zip ./changelog.txt
zip $1-linux64.zip ./readme.txt
rm ./CGenius

# now create the 64-bit ubuntu package
cd distro 
./genubuntuamd64.sh
cd ..
mv distro/CGenius_x64.deb $1-x64.deb

# Linux 32-bit Version
cp build/Linux32/CGenius ./CGenius
cp build/Linux32/CGenius ./distro/Ubuntu/usr/games

cd vfsroot
git archive -o ../$1-linux32.zip HEAD
cd ..

zip $1-linux32.zip ./CGenius
zip $1-linux32.zip ./changelog.txt
zip $1-linux32.zip ./readme.txt
rm ./CGenius

# now create the 32-bit ubuntu package
cd distro 
./genubuntui386.sh
cd ..
mv distro/CGenius_i386.deb $1-i386.deb

# upload the packages
#rsync -e ssh $1-win32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/#project/c/cl/clonekeenplus/
#rsync -e ssh C$1-linux32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/#frs/project/c/cl/clonekeenplus/
#rsync -e ssh $1-linux64.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/#frs/project/c/cl/clonekeenplus/
#rsync -e ssh $1-i386.deb gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/#project/c/cl/clonekeenplus/
#rsync -e ssh $1-x64.deb gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/#project/c/cl/clonekeenplus/
