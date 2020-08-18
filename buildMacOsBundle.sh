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

VERSION=`bash get_version.sh`
genisoimage -V CGenius -D -R -apple -no-pad -o CGenius-${VERSION}-MacOS.dmg CGenius.app
# Cleanups
rm -rf CGenius.app
