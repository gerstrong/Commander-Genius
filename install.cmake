########################################################################
############### Prepare the installation of the compiled stuff here!

IF(BUILD_TYPE STREQUAL WIN32)
SET(SHAREDIR CGenius)
SET(APPDIR CGenius)
ELSE(BUILD_TYPE STREQUAL WIN32)
SET(SHAREDIR share/CommanderGenius)
SET(APPDIR games)
ENDIF(BUILD_TYPE STREQUAL WIN32)

# This will install the application itself
INSTALL(TARGETS commandergenius 
	DESTINATION ${APPDIR})

# This will copy the resources files to the proper directory
INSTALL(DIRECTORY vfsroot/ 
	DESTINATION ${SHAREDIR})

# This will copy the readme file. 
INSTALL(FILES readme.txt 
	DESTINATION ${SHAREDIR})

# This will copy the readme file. 
INSTALL(FILES changelog.txt 
	DESTINATION ${SHAREDIR})

# Windows might not have those dlls so we ship them with the packages
IF(BUILD_TYPE STREQUAL WIN32)

INSTALL(FILES libogg-0.dll libvorbis-0.dll libvorbisenc-2.dll libvorbisfile-3.dll SDL.dll
	DESTINATION ${APPDIR})
ENDIF(BUILD_TYPE STREQUAL WIN32)

