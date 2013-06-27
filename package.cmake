OPTION(CREATE_DEBS "Will create DEBs" No)
OPTION(CREATE_RPMS "Will create RPMs" No)

# Now let's setup the package for installation. You can create the package of the chosen build
# running make package, and if needed just runs that package
# For Linux it will create Debian Packages and replace the manual script I have been using in the past.

## Generation of Packages.
# Generation of Packages. Common stuff and Basic variables
SET(CPACK_PACKAGE_NAME "CGenius")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The ultimate Commander Keen interpreter")
SET(CPACK_PACKAGE_DESCRIPTION "Commander-Genius aka CloneKeenPlus is an engine which interprets Commander Keen 1-3 (Vorticons Series).\n .\n As fans and developers we try to implement new features, improve the game play and give the feeling, you are playing the original game.\n .\n Obviously you need the game data of the games to play it.\n Commander Keen 1 comes included in this package.\n Episode 2 and 3 are registered versions which can be purchased by 3D Realms or Apogee, or bought by someone else.\n If you still have the old games of the registered version, then give it a try. You will be surprised, how well they will look!\n Commander Keen 4 support is under heavy development. Some parts are suddenly starting to work.")
SET(CPACK_PACKAGE_VENDOR "The Commmander-Genius Team")
SET(CPACK_PACKAGE_VERSION ${CG_VERSION})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${BUILD_TARGET}")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT")


IF(BUILD_TARGET STREQUAL WIN32)
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${APPDIR})
	SET(CPACK_GENERATOR "ZIP;NSIS")
ELSE(BUILD_TARGET STREQUAL WIN32)
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/games")
	SET(CPACK_GENERATOR "TGZ")
ENDIF(BUILD_TARGET STREQUAL WIN32)

IF(BUILD_TARGET STREQUAL WIN32)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "CGenius.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "Commander-Genius")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\clonekeenplus.sourceforge.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\clonekeenplus.sourceforge.net")
  SET(CPACK_NSIS_CONTACT "gerstrong@gmail.com")
  SET(CPACK_NSIS_MODIFY_PATH OFF)
  set(CPACK_NSIS_CREATE_ICONS "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Commander Genius.lnk' '\$INSTDIR\\\\CGenius\\\\CGenius.exe'")
ENDIF()

SET(CPACK_PACKAGE_EXECUTABLES "CGenius" "Commander Genius")

# This section is only called if the user wants to get debian packages created
IF(CREATE_DEBS)
	IF(BUILD_TARGET STREQUAL WIN32)
		MESSAGE("NOTE: Windows does not use DEB Packages. Those won't be build here...")
	ELSE(BUILD_TARGET STREQUAL WIN32)
		SET(CPACK_GENERATOR "DEB;${CPACK_GENERATOR}") # For Debian, Ubuntu etc...
		SET(CPACK_DEBIAN_PACKAGE_NAME "CGenius")
		SET(CPACK_DEBIAN_PACKAGE_MAINTAINER  "Gerhard Stein <gerstrong@gmail.com>")
		SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n .\n ${CPACK_PACKAGE_DESCRIPTION}")

		IF(BUILD_ARCH STREQUAL amd64)
			SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
		ELSEIF(BUILD_ARCH STREQUAL i386)
			SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
		ENDIF(BUILD_ARCH STREQUAL amd64)


		SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libvorbis0a (>= 1.2.3), libvorbisenc2 (>> 1.2.3), libvorbisfile3 (>= 1.2.3), libsdl1.2debian (>= 1.2.14), libgl1-mesa-glx (>= 7.7.1), libsdl-image1.2 (>= 1.2.10-3)")
		SET(CPACK_DEBIAN_PACKAGE_SECTION "games")
		SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
	ENDIF(BUILD_TARGET STREQUAL WIN32)
ENDIF(CREATE_DEBS)


# This section is only called if the user wants to get debian packages created
IF(CREATE_RPMS)
	IF(BUILD_TARGET STREQUAL WIN32)
		MESSAGE("NOTE: Windows does not use RPM Packages. Those won't be build here...")
	ELSE(BUILD_TARGET STREQUAL WIN32)
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
		SET(CPACK_RPM_PACKAGE_REQUIRES "SDL >= 1.2, libvorbis >= 1.3, mesa-libGL >= 7.7")
	ENDIF(BUILD_TARGET STREQUAL WIN32)
ENDIF(CREATE_RPMS)

INCLUDE(CPack)

MESSAGE( "CPACK_PACKAGE_VERSION = ${CPACK_PACKAGE_VERSION}" )


MESSAGE( "Build system is prepared. To Build the project just type \"make\"" )
MESSAGE( "If you want to create the installation package just type \"make package\" after you build the project" )

