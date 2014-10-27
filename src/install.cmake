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
#SET(GAMES_SHAREDIR "${SHAREDIR}" CACHE PATH "Game data root dir")
SET(GAMES_SHAREDIR "share/games/" CACHE PATH "Game data root dir")
SET(DATADIR "${GAMES_SHAREDIR}/commandergenius")
SET(APPDIR games CACHE PATH "Binary destination")
SET(DOCDIR ${DATADIR} CACHE PATH "Docs destination")
ENDIF(BUILD_TARGET STREQUAL WIN32)

# This will install the application itself
INSTALL(TARGETS CGeniusExe 
	DESTINATION ${APPDIR})

# This will copy the resources files to the proper directory
IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/../vfsroot")
	INSTALL(DIRECTORY ../vfsroot/ 
		DESTINATION ${DATADIR})
ENDIF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/../vfsroot")

# This will copy the readme file. 
INSTALL(FILES "${CMAKE_BINARY_DIR}/README"
	DESTINATION ${DOCDIR})

# This will copy the changelog file. 
INSTALL(FILES "${CMAKE_SOURCE_DIR}/changelog.txt" 
	DESTINATION ${DOCDIR})

# Windows might not have those dlls so we ship them with the packages
IF(BUILD_TARGET STREQUAL WIN32)

INSTALL(FILES SDL_image.dll libjpeg-8.dll libpng15-15.dll libtiff-5.dll libwebp-2.dll zlib1.dll libogg-0.dll libvorbis-0.dll libvorbisenc-2.dll libvorbisfile-3.dll SDL.dll libgcc_s_sjlj-1.dll libstdc++-6.dll
	DESTINATION ${APPDIR})
ENDIF(BUILD_TARGET STREQUAL WIN32)

