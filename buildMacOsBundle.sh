#!/bin/bash
# Build script for MacOs Bundle
# More setup hints here: https://www.steinzone.de/wordpress/cross-building-commander-genius-fuer-macos-using-a-linux-system/

# create bundle structure
mkdir CGenius.app
mkdir -p CGenius.app/Contents
mkdir -p CGenius.app/Contents/libs
mkdir -p CGenius.app/Contents/MacOS
mkdir -p CGenius.app/Contents/Resources

cp src/Info.plist CGenius.app/Contents
cp src/CGeniusExe CGenius.app/Contents/MacOS
cp src/cglogo.icns CGenius.app/Contents/Resources
chmod +x launchCGonMacOs.sh
cp launchCGonMacOs.sh CGenius.app/Contents/MacOS

bash makebundleable.sh CGenius.app/Contents/MacOS/CGeniusExe

# Pack it to an DMG (15 MB) (Just be sure you have installed hfsprogs)
dd if=/dev/zero of=/tmp/my_application.dmg bs=1M count=15 status=progress
mkfs.hfsplus -v Install /tmp/my_application.dmg

# TODO: This should work for a non-root user.
sudo mkdir -pv /mnt/tmp
sudo mount -o loop /tmp/my_application.dmg /mnt/tmp
sudo cp -av CGenius.app /mnt/tmp
sudo umount /mnt/tmp
mv /tmp/my_application.dmg ./
# TODO: Here we should have the version passed to the filename
VERSION=`bash get_version.sh`
mv my_application.dmg CommanderGenius-${VERSION}-MacOS.dmg

# Cleanups
#rm -rf CGenius.app

