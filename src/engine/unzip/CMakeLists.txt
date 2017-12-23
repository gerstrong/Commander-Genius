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

