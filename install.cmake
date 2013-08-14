########################################################################
############### Prepare the installation of the compiled stuff here!

IF(BUILD_TARGET STREQUAL WIN32)
SET(DATADIR CGenius)
SET(APPDIR CGenius)
ELSE(BUILD_TARGET STREQUAL WIN32)
SET(SHAREDIR share CACHE PATH "System share dir location")
# seperate SHAREDIR and GAMES_SHAREDIR
# for cases where we want /usr/share/games for the data
# and /usr/share for desktop-files/pixmaps and such
SET(GAMES_SHAREDIR "${SHAREDIR}/games" CACHE PATH "Game data root dir")
SET(DATADIR "${GAMES_SHAREDIR}/commandergenius")
SET(APPDIR games CACHE PATH "Binary destination")
SET(DOCDIR ${DATADIR} CACHE PATH "Docs destination")
ENDIF(BUILD_TARGET STREQUAL WIN32)

# This will install the application itself
INSTALL(TARGETS CommanderGenius 
	DESTINATION ${APPDIR})

# This will copy the resources files to the proper directory
INSTALL(DIRECTORY vfsroot/ 
	DESTINATION ${DATADIR})

# This will copy the readme file. 
INSTALL(FILES README 
	DESTINATION ${DOCDIR})

# This will copy the readme file. 
INSTALL(FILES changelog.txt 
	DESTINATION ${DOCDIR})

# Windows might not have those dlls so we ship them with the packages
IF(BUILD_TARGET STREQUAL WIN32)

INSTALL(FILES SDL_image.dll libjpeg-8.dll libpng15-15.dll libtiff-5.dll libwebp-2.dll zlib1.dll libogg-0.dll libvorbis-0.dll libvorbisenc-2.dll libvorbisfile-3.dll SDL.dll libgcc_s_sjlj-1.dll libstdc++-6.dll
	DESTINATION ${APPDIR})
ENDIF(BUILD_TARGET STREQUAL WIN32)

