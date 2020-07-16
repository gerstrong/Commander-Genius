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


IF(WIN32)
ELSE(WIN32)
# Install desktop icon and file
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.png 
	DESTINATION "${ICONDIR}/512x512/apps")

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../share/cgenius.desktop
	DESTINATION "${DESKTOPFILESDIR}")
ENDIF(WIN32)

	
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

    option(PULL_DLLS "If your host system is some linux system with specific libraries, enable this. Otherwise it will copy the already provided dlls " OFF)

    if(PULL_DLLS)
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libcurl-4.dll" DESTINATION "${APPDIR}")
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/lib/libSDL2.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/lib/libSDL2_image.dll" DESTINATION "${APPDIR}")
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libjpeg-9.dll" DESTINATION "${APPDIR}")
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libpng16-16.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/lib/libSDL2_mixer.dll" DESTINATION "${APPDIR}")
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libmpg123-0.dll" DESTINATION "${APPDIR}")
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libogg-0.dll" DESTINATION "${APPDIR}")
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libvorbis-0.dll" DESTINATION "${APPDIR}")
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libvorbisfile-3.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/lib/libSDL2_ttf.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "/usr/lib/gcc/x86_64-w64-mingw32/8.3-win32/libgcc_s_seh-1.dll" DESTINATION "${APPDIR}")
    else()
        INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/dlls/" DESTINATION "${APPDIR}")
    endif(PULL_DLLS)
ENDIF(WIN32)   

