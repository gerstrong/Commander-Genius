find_package(PkgConfig)

#if ( PKGCONFIG_FOUND AND NOT WIN32 )
#    pkg_check_modules(ZLIB minizip)
#else ( PKGCONFIG_FOUND AND NOT WIN32 )
#    unset(ZLIB_FOUND)
#endif ( PKGCONFIG_FOUND AND NOT WIN32 )

if ( ZLIB_FOUND )
    set(CXXSRC miniunz.c)
    unset(HSRC)

else ( ZLIB_FOUND )
    find_package(ZLIB REQUIRED)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

    set(CXXSRC
        miniunz.c
        ioapi.c
        iowin32.c
        unzip.c)

    set(HSRC
        ioapi.h
        iowin32.h
        unzip.h)
endif ( ZLIB_FOUND )

if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
else ( ZLIB_FOUND )
    Message(FATAL_ERROR "Zlib development packages not found!")
endif( ZLIB_FOUND )

add_library(unzip STATIC ${CXXSRC} ${HSRC})

target_compile_definitions(unzip PRIVATE -DIOAPI_NO_64)

# Link zlib
target_link_libraries( unzip ${ZLIB_LIBRARIES} )

