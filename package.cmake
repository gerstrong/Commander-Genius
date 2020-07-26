OPTION(CREATE_DEBS "Will create DEBs" No)
OPTION(CREATE_RPMS "Will create RPMs" No)

SET(CGENIUS_DESKTOP_DIR "share/applications/")


# Now let's setup the package for installation. You can create the package of the chosen build
# running make package, and if needed just runs that package
# For Linux it will create Debian Packages and replace the manual script I have been using in the past.

## Generation of Packages.
# Common stuff and Basic variables
SET(CPACK_PACKAGE_NAME "CGenius")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The ultimate Commander Keen interpreter")
SET(CPACK_PACKAGE_DESCRIPTION "Commander-Genius is an engine which interprets Commander Keen 1-6 (Vorticons and Galaxy Series) and Dreams.\n\nAs fans and developers we try to implement new features, improve the gameplay and give the feeling, you are playing the original game.\n .\n Obviously you need the game data of the games to play it.\n Commander Keen 1 and 4 come included in this package.\n Episode 2, 3, 5 and 6 are registered versions which can be purchased by 3D Realms or Apogee, or bought by someone else.\n If you still have the old games of the registered version, then give it a try. You will be surprised, how well they will look!\n")
SET(CPACK_PACKAGE_VENDOR "The Commmander-Genius Team")

SET(CPACK_PACKAGE_VERSION ${CG_VERSION})
option(APPEND_SHA "Appends short git SHA to  " OFF)
if(APPEND_SHA)
    SET(CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION}-${GIT_SHA})
endif(APPEND_SHA)


SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_BUILD_TYPE}")


IF(WIN32)
	SET(APPDIR CGenius)
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${APPDIR})
        FILE(GLOB CACHE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/vfsroot/cache/*.png")
        INSTALL(FILES ${CACHE_FILES} DESTINATION ${DOCDIR}/cache)
ELSE(WIN32)
        SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT")
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/games")
	SET(CPACK_GENERATOR "TGZ")	
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/share/cgenius.desktop DESTINATION ${CGENIUS_DESKTOP_DIR})
ENDIF(WIN32)


IF(WIN32)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backslashes.    
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "CGenius.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "Commander-Genius")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\clonekeenplus.sourceforge.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\clonekeenplus.sourceforge.net")
  SET(CPACK_NSIS_CONTACT "gerstrong@gmail.com")
  SET(CPACK_NSIS_MODIFY_PATH OFF)
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_BUILD_TYPE}-win32")
  set(CPACK_NSIS_CREATE_ICONS "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Commander Genius.lnk' '\$INSTDIR\\\\CGenius\\\\CGenius.exe'")
ENDIF()

SET(CPACK_PACKAGE_EXECUTABLES "CGenius" "Commander Genius")

# This section is only called if the user wants to get debian packages created
IF(CREATE_DEBS)
	IF(WIN32)
		MESSAGE("NOTE: Windows does not use DEB Packages. Those won't be build here...")
	ELSE(WIN32)
		SET(CPACK_GENERATOR "DEB;${CPACK_GENERATOR}") # For Debian, Ubuntu etc...
		SET(CPACK_DEBIAN_PACKAGE_NAME "CGenius")
		SET(CPACK_DEBIAN_PACKAGE_MAINTAINER  "Gerhard Stein <gerstrong@gmail.com>")
		SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n .\n ${CPACK_PACKAGE_DESCRIPTION}")

		IF(BUILD_ARCH STREQUAL amd64)
			SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
		ELSEIF(BUILD_ARCH STREQUAL i386)
			SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
		ENDIF(BUILD_ARCH STREQUAL amd64)
                SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-2.0-0 (>= 2.0.0), libgl1-mesa-glx (>= 7.7.1), libsdl2-image-2.0-0 (>= 2.0), libsdl2-mixer-2.0-0 (>= 2.0)")
		SET(CPACK_DEBIAN_PACKAGE_SECTION "games")
		SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
	ENDIF(WIN32)
ENDIF(CREATE_DEBS)


# This section is only called if the user wants to get debian packages created
IF(CREATE_RPMS)
	IF(WIN32)
		MESSAGE("NOTE: Windows does not use RPM Packages. Those won't be build here...")
	ELSE(WIN32)
		SET(CPACK_GENERATOR "RPM;${CPACK_GENERATOR}") # For Fedora, OpenSuse, etc.
		SET(CPACK_RPM_PACKAGE_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
		SET(CPACK_RPM_PACKAGE_NAME "CGenius")
		SET(CPACK_RPM_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})

		IF(BUILD_TARGET STREQUAL LINUX64)
			SET(CPACK_RPM_PACKAGE_ARCHITECTURE x86_64)
		ENDIF(BUILD_TARGET STREQUAL LINUX64)
		
		SET(CPACK_RPM_PACKAGE_RELEASE 1)
		SET(CPACK_RPM_PACKAGE_LICENSE "GPL 2")
		SET(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
		SET(CPACK_RPM_PACKAGE_VENDOR "${CPACK_PACKAGE_VENDOR}")
		SET(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n .\n ${CPACK_PACKAGE_DESCRIPTION}")
                SET(CPACK_RPM_PACKAGE_REQUIRES "SDL >= 2.0, SDL_image >= 2.0, SDL_mixer >= 2.0, mesa-libGL >= 7.7")
	ENDIF(WIN32)
ENDIF(CREATE_RPMS)


INCLUDE(CPack)

MESSAGE( STATUS "CPACK_PACKAGE_VERSION = ${CPACK_PACKAGE_VERSION}" )

