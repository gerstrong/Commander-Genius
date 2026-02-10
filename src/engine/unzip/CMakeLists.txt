find_package(PkgConfig)

#if ( PKGCONFIG_FOUND AND NOT WIN32 )
#    pkg_check_modules(ZLIB minizip)
#else ( PKGCONFIG_FOUND AND NOT WIN32 )
#    unset(ZLIB_FOUND)
#endif ( PKGCONFIG_FOUND AND NOT WIN32 )

find_path(MINIZIP_INCLUDE_DIR unzip.h) 
find_library(MINIZIP_LIBRARY minizip) 
if(MINIZIP_INCLUDE_DIR AND MINIZIP_LIBRARY) 
	set(ZLIB_FOUND TRUE) 
else() 
	set(ZLIB_FOUND FALSE) 
endif()


if ( ZLIB_FOUND )
    set(CXXSRC miniunz.c)
    unset(HSRC)

else ( ZLIB_FOUND )

	message(STATUS "$$$$$$$$$$ Will find zlib")
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

