# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 
    
cmake_minimum_required(VERSION 3.5)

Project(GsKit_base_lua)

set (CMAKE_C_STANDARD 99)

# TODO: Check if this also works on Windows and MacOS
add_definitions(-DLUA_USE_POSIX)

set(HEADERS lapi.h     ldebug.h  llex.h     lopcodes.h  lstring.h  lua.h      lzio.h
lauxlib.h  ldo.h     llimits.h  lparser.h   ltable.h   lualib.h
lcode.h    lfunc.h   lmem.h     lprefix.h   ltm.h      lundump.h
lctype.h   lgc.h     lobject.h  lstate.h    luaconf.h  lvm.h
)

set(SRCS lapi.c      lcorolib.c  ldump.c   llex.c      lopcodes.c  lstrlib.c
lauxlib.c   lctype.c    lfunc.c   lmathlib.c  loslib.c    ltable.c   lundump.c
lbaselib.c  ldblib.c    lgc.c     lmem.c      lparser.c   ltablib.c  lutf8lib.c
lbitlib.c   ldebug.c    linit.c   loadlib.c   lstate.c    ltm.c      lvm.c
lcode.c     ldo.c       liolib.c  lobject.c   lstring.c   lua.c      lzio.c
)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})
