#!/bin/sh

# script taken from Pidgin-MacOSX project
# by Albert Zeyer - www.az2000.de

# builds a DMG for Commander Genius


# Creates a disk image (dmg) on Mac OS X from the command line.
# usage:
#    mkdmg <volname> <vers> <srcdir>
#
# Where <volname> is the name to use for the mounted image, <vers> is the version
# number of the volume and <srcdir> is where the contents to put on the dmg are.
#
# The result will be a file called <volname>-<vers>.dmg
mkdmg() {

	if [ $# != 3 ]; then
	 echo "usage: mkdmg.sh volname vers srcdir"
	 exit 0
	fi

	VOL="$1"
	VER="$2"
	FILES="$3"

	DMG="tmp-$VOL.dmg"

	# create temporary disk image and format, ejecting when done
	SIZE=$(du -sk ${FILES} | cut -f 1)
	SIZE=$((${SIZE}/1000+5))
	hdiutil create "$DMG" -megabytes ${SIZE} -ov -type UDIF -fs HFS+ -volname "$VOL"

	# mount and copy files onto volume
	hdid "$DMG"
	echo -n "copying files ... "
	cp -R "${FILES}"/* "/Volumes/$VOL/"
	echo "ready"
	hdiutil eject "/Volumes/$VOL"
	#osascript -e "tell application "Finder" to eject disk "$VOL"" && 

	# convert to compressed image, delete temp image
	rm -f "${VOL}-${VER}.dmg"
	hdiutil convert "$DMG" -format UDZO -o "${VOL}-${VER}.dmg"
	rm -f "$DMG"

}


cd "$(dirname "$0")"
rsync -a --delete build/Release/Commander\ Genius.app dmg/
mkdmg CommanderGenius "$(egrep -E ".*#define.*CGVERSION.*" ../../version.h | cut -d "\"" -f 2)" dmg
