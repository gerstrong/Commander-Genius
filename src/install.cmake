########################################################################
############### Prepare the installation of the compiled stuff here!

# This will install the application itself
INSTALL(TARGETS CGeniusExe
    DESTINATION ${APPDIR})


IF(WIN32)
ELSE(WIN32)
# Install desktop icon and file
foreach(X IN ITEMS 16 32 48 64 128 256 512)
   INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../share/cg.${X}.png
                 DESTINATION "${ICONDIR}/${X}x${X}/apps")
endforeach()

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../share/cgenius.desktop
    DESTINATION "${DESKTOPFILESDIR}")

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../share/metainfo/io.sourceforge.clonekeenplus.appdata.xml
        DESTINATION "${SHAREDIR}/metainfo")

ENDIF(WIN32)


# This will copy the resources files to the proper directory
IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/vfsroot")
    INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/vfsroot/"
        DESTINATION ${DATADIR})
ELSE(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/vfsroot")
    Message ("Error: Missing ${CMAKE_SOURCE_DIR}/vfsroot")
ENDIF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/vfsroot")

# This will copy the readme file.
INSTALL(FILES "${CMAKE_BINARY_DIR}/README.md"
    DESTINATION ${DOCDIR})

# This will copy the changelog file.
INSTALL(FILES "${CMAKE_SOURCE_DIR}/changelog.txt"
    DESTINATION ${DOCDIR})

# Windows might not have those dlls so we ship them with the packages
IF(WIN32)

    option(PULL_DLLS "If your host system is some linux system with specific libraries, enable this. Otherwise it will copy the already provided dlls " OFF)

    if(PULL_DLLS)

        # OpenSSL 3
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libcrypto-3-x64.dll" DESTINATION "${APPDIR}")
        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libssl-3-x64.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libcurl-4.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/SDL2.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/SDL2_image.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/SDL2_mixer.dll" DESTINATION "${APPDIR}")
        #INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libmpg123-0.dll" DESTINATION "${APPDIR}")
        #INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libogg-0.dll" DESTINATION "${APPDIR}")
        #INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libvorbis-0.dll" DESTINATION "${APPDIR}")
        #INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libvorbisfile-3.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/SDL2_ttf.dll" DESTINATION "${APPDIR}")
        #INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/bin/libfreetype-6.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "${CMAKE_FIND_ROOT_PATH}/lib/libwinpthread-1.dll" DESTINATION "${APPDIR}")

        INSTALL(FILES "/usr/lib/gcc/x86_64-w64-mingw32/12-win32/libgcc_s_seh-1.dll" DESTINATION "${APPDIR}")
        INSTALL(FILES "/usr/lib/gcc/x86_64-w64-mingw32/12-win32/libstdc++-6.dll" DESTINATION "${APPDIR}")

    else()
        INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/dlls/" DESTINATION "${APPDIR}")
    endif(PULL_DLLS)
ENDIF(WIN32)

