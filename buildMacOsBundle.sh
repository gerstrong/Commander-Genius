#!/bin/bash
# Build script for MacOs Bundle

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

# x86_64-apple-darwin14-otool -L src/CGeniusExe
# Lists which libraries are required. I got this the last time
#
#	/opt/local/lib/libSDL2_mixer-2.0.0.dylib (compatibility version 3.0.0, current version 3.2.0)
#	/opt/local/lib/libSDL2-2.0.0.dylib (compatibility version 13.0.0, current version 13.0.0)
#	/System/Library/Frameworks/Cocoa.framework/Versions/A/Cocoa (compatibility version 1.0.0, current version 21.0.0)
#	/opt/local/lib/libSDL2_image-2.0.0.dylib (compatibility version 3.0.0, current version 3.3.0)
#	/opt/local/lib/libSDL2_ttf-2.0.0.dylib (compatibility version 15.0.0, current version 15.1.0)
#	/usr/lib/libz.1.dylib (compatibility version 1.0.0, current version 1.2.5)
#	/usr/lib/libcurl.4.dylib (compatibility version 7.0.0, current version 8.0.0)
#	/usr/lib/libc++.1.dylib (compatibility version 1.0.0, current version 120.0.0)
#	/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1213.0.0)
#

# Let's those from the right directory
cp -L ${OSXOPT}/local/lib/libSDL2_mixer-2.0.0.dylib CGenius.app/Contents/MacOS
cp -L ${OSXOPT}/local/lib/libSDL2-2.0.0.dylib CGenius.app/Contents/MacOS
cp -L ${OSXOPT}/local/lib/libSDL2_image-2.0.0.dylib CGenius.app/Contents/MacOS
cp -L ${OSXOPT}/local/lib/libSDL2_ttf-2.0.0.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/SDK/MacOSX10.10.sdk/usr/lib/libcurl.4.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/SDK/MacOSX10.10.sdk/usr/lib/libz.1.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/SDK/MacOSX10.10.sdk/usr/lib/libc++.1.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/SDK/MacOSX10.10.sdk/usr/lib/libSystem.B.dylib CGenius.app/Contents/MacOS

# Those are loaded at runtime by SDL2_Mixer
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libFLAC.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libmodplug.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libmpg123.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libogg.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libopus.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libvorbis.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libvorbisenc.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libvorbisfile.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libopusfile.dylib CGenius.app/Contents/MacOS
cp -L ${OSXCROSSPATH}/target/macports/pkgs/opt/local/lib/libopusurl.dylib CGenius.app/Contents/MacOS


x86_64-apple-darwin14-install_name_tool -change /opt/local/lib/libSDL2-2.0.0.dylib @executable_path/libSDL2-2.0.0.dylib CGenius.app/Contents/MacOS/CGeniusExe
x86_64-apple-darwin14-install_name_tool -change /opt/local/lib/libSDL2_mixer-2.0.0.dylib @executable_path/libSDL2_mixer-2.0.0.dylib CGenius.app/Contents/MacOS/CGeniusExe
x86_64-apple-darwin14-install_name_tool -change /opt/local/lib/libSDL2_image-2.0.0.dylib @executable_path/libSDL2_image-2.0.0.dylib CGenius.app/Contents/MacOS/CGeniusExe
x86_64-apple-darwin14-install_name_tool -change /opt/local/lib/libSDL2_ttf-2.0.0.dylib @executable_path/libSDL2_ttf-2.0.0.dylib CGenius.app/Contents/MacOS/CGeniusExe
x86_64-apple-darwin14-install_name_tool -change /usr/lib/libcurl.4.dylib @executable_path/libcurl.4.dylib CGenius.app/Contents/MacOS/CGeniusExe
x86_64-apple-darwin14-install_name_tool -change /usr/lib/libz.1.dylib @executable_path/libz.1.dylib CGenius.app/Contents/MacOS/CGeniusExe
x86_64-apple-darwin14-install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib CGenius.app/Contents/MacOS/CGeniusExe
x86_64-apple-darwin14-install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib CGenius.app/Contents/MacOS/CGeniusExe

#x86_64-apple-darwin14-install_name_tool -add_rpath @executable_path CGenius.app/Contents/MacOS/CGeniusExe

x86_64-apple-darwin14-install_name_tool -add_rpath @executable_path../libs CGenius.app/Contents/MacOS/CGeniusExe

# Pack it to an DMG (10 MB) (Just be sure you have installed hfsprogs)
dd if=/dev/zero of=/tmp/my_application.dmg bs=1M count=25 status=progress
mkfs.hfsplus -v Install /tmp/my_application.dmg

# TODO: This should work for a normal user.
sudo mkdir -pv /mnt/tmp
sudo mount -o loop /tmp/my_application.dmg /mnt/tmp
sudo cp -av CGenius.app /mnt/tmp
sudo umount /mnt/tmp
mv /tmp/my_application.dmg ./
# TODO: Here we should have the version passed to the filename
mv my_application.dmg CommanderGenius.dmg

# Cleanups
#rm -rf CGenius.app

