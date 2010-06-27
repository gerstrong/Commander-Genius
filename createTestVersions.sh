#!/bin/sh

# put date on the filename
dt=`date +%b%d%y-%H%M`

# Windows Version
cp build/Win32/CGenius.exe ./CGenius.exe

cd vfsroot
git archive -o ../CG-SVN-${dt}-win32.zip HEAD
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
cp build/Linux64/CGenius ./CGenius
cp build/Linux64/CGenius ./distro/Ubuntu/usr/games

cd vfsroot
git archive -o ../CG-SVN-${dt}-linux64.zip HEAD
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
cp build/Linux32/CGenius ./CGenius
cp build/Linux32/CGenius ./distro/Ubuntu/usr/games

cd vfsroot
git archive -o ../CG-SVN-${dt}-linux32.zip HEAD
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

# upload the packages
rsync -e ssh CG-SVN-${dt}-win32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
rsync -e ssh CCG-SVN-${dt}-linux32.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
rsync -e ssh CG-SVN-${dt}-linux64.zip gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
rsync -e ssh CG-SVN-${dt}-i386.deb gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
rsync -e ssh CG-SVN-${dt}-x64.deb gerstrong,clonekeenplus@frs.sourceforge.net:/home/frs/project/c/cl/clonekeenplus/
