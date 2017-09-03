#!/bin/sh

VERSION=1.9.8
SUBVER=ppa3
KEY="0x6A1F4CC6"

DST=../cgenius_${VERSION}

echo "Preparing assets for directory..."

# Be careful, not everything can be copied!
mkdir -p ${DST}
cp -r CMake ${DST}
cp -r debian ${DST}
cp -r docs ${DST}
cp -r lib ${DST}
cp -r share ${DST}
cp -r src ${DST}
cp -r tools ${DST}
cp -r vfsroot ${DST}
cp -r share ${DST}
cp * ${DST}

# Build it the debian way
cd ${DST}

# Sources Packages signed for PPA
debuild -S -k${KEY}

cd ..

#debuild -us -uc
dput ppa:gerstrong/cgenius commandergenius_${VERSION}${SUBVER}_source.changes
