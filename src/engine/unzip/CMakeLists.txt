find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
else ( ZLIB_FOUND )
    Message(FATAL_ERROR "Zlib development packages not found!")
endif( ZLIB_FOUND )




set(CXXSRC
    miniunz.c
    ioapi.c
    iowin32.c
    unzip.c)

set(HSRC
    ioapi.h
    iowin32.h
    unzip.h)

add_library(unzip ${CXXSRC} ${HSRC})


# Link zlib
target_link_libraries( unzip ${ZLIB_LIBRARIES} )

