#!/bin/bash

find ./ -name CMakeFiles -exec /bin/rm -rf '{}' +
find ./ -name _CPack_Packages -exec /bin/rm -rf '{}' +

