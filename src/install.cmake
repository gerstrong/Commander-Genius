########################################################################
############### Prepare the installation of the compiled stuff here!

IF(WIN32)
SET(DATADIR CGenius)
SET(APPDIR CGenius)
SET(GAMES_SHAREDIR "${APPDIR}" CACHE PATH "Game data root dir")
SET(DOCDIR ${DATADIR} CACHE PATH "Docs destination")
ELSE(WIN32)
SET(SHAREDIR "/usr/share" CACHE PATH "System share dir location")
SET(GAMES_SHAREDIR "${SHAREDIR}/games/" CACHE PATH "Game data root dir")
SET(DATADIR "${GAMES_SHAREDIR}/commandergenius")
SET(APPDIR games CACHE PATH "Binary destination")
SET(DOCDIR ${DATADIR} CACHE PATH "Docs destination")
SET(ICONDIR "${SHAREDIR}/icons/hicolor" CACHE PATH "Standard icon installation dir")
SET(DESKTOPFILESDIR "${SHAREDIR}/applications" CACHE PATH "Application installation dir")
ENDIF(WIN32)

# This will install the application itself
INSTALL(TARGETS CGeniusExe 
	DESTINATION ${APPDIR})


# Install desktop icon and file
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.png 
	DESTINATION "${ICONDIR}/512x512/apps")


INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../share/cgenius.desktop
	DESTINATION "${DESKTOPFILESDIR}")

	
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

