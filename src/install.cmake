########################################################################
############### Prepare the installation of the compiled stuff here!

IF(WIN32)
SET(DATADIR CGenius)
SET(APPDIR CGenius)
SET(GAMES_SHAREDIR "${APPDIR}" CACHE PATH "Game data root dir")
SET(DOCDIR ${DATADIR} CACHE PATH "Docs destination")
ELSE(WIN32)
SET(SHAREDIR share CACHE PATH "System share dir location")
# seperate SHAREDIR and GAMES_SHAREDIR
# for cases where we want /usr/share/games for the data
# and /usr/share for desktop-files/pixmaps and such
#SET(GAMES_SHAREDIR "${SHAREDIR}" CACHE PATH "Game data root dir")
SET(GAMES_SHAREDIR "share/games/" CACHE PATH "Game data root dir")
SET(DATADIR "${GAMES_SHAREDIR}/commandergenius")
SET(APPDIR games CACHE PATH "Binary destination")
SET(DOCDIR ${DATADIR} CACHE PATH "Docs destination")
ENDIF(WIN32)

# This will install the application itself
INSTALL(TARGETS CGeniusExe 
	DESTINATION ${APPDIR})
	
# This will copy the resources files to the proper directory
IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/vfsroot")
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/vfsroot/"
		DESTINATION ${DATADIR})
ELSE(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/vfsroot")
	Message ("Error: Missing ${CMAKE_SOURCE_DIR}/vfsroot")
ENDIF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/vfsroot")

# This will copy the readme file. 
INSTALL(FILES "${CMAKE_BINARY_DIR}/README"
	DESTINATION ${DOCDIR})

# This will copy the changelog file. 
INSTALL(FILES "${CMAKE_SOURCE_DIR}/changelog.txt" 
	DESTINATION ${DOCDIR})

# Windows might not have those dlls so we ship them with the packages
IF(WIN32)
    INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/dlls/" DESTINATION "${APPDIR}")
ENDIF(WIN32)   

