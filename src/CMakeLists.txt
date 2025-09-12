# Platform specific things can be put here
# Compilers and other specific variables can be found here:
# http://www.cmake.org/Wiki/CMake_Useful_Variables

OPTION(DOWNLOADER "Internal Downloader" Yes)


IF(WIN32)
        SET(BUILD_TARGET WIN32)
        ADD_DEFINITIONS(-DWIN32)
        ADD_DEFINITIONS(-DTARGET_WIN32)
ELSEIF(UNIX)
        SET(BUILD_TARGET LINUX)
ENDIF()

IF(APPLE)
    SET(BUILD_TARGET APPLE)
endif()

IF(NOT DEFINED BUILD_TARGET)
    MESSAGE( STATUS "WARNING: You did not specify the build type with the -DBUILD_TARGET= Parameter. ${BUILD_TARGET} has been choosen" )
    MESSAGE( STATUS "Available builds are: WIN32, LINUX GP2X. WIZ, CAANOO, DINGOO, NANONOTE" )
ENDIF(NOT DEFINED BUILD_TARGET)


SET(USRDIR /usr)

include(oldArch.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Alignment error report is important for android builds
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=cast-align")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=cast-align")

# Main includes
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/GsKit)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake)

# If we want to debug set the proper flags or have release etc.
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
        ADD_DEFINITIONS(-DDEBUG)
        ADD_DEFINITIONS(-g)
        ADD_DEFINITIONS(-O0)
        ADD_DEFINITIONS(-Wall)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
        ADD_DEFINITIONS(-DRELEASE)
        ADD_DEFINITIONS(-O2)
ENDIF()


# Make sure the linker can find the GsKit library once it is built.
link_directories (${CMAKE_BINARY_DIR}/GsKit)

MESSAGE( STATUS "Internal Downloader: ${DOWNLOADER}" )

add_subdirectory(engine)

if(GP2X)
    add_subdirectory(sys)
endif(GP2X)

add_subdirectory(fileio)
add_subdirectory(dialog)

# Get all the o-files from the different targets and link'em
get_property(cg_obj_libs GLOBAL PROPERTY CG_OBJ_LIBS)

if(OPT_PATH)
    MESSAGE(STATUS "OPT_PATH = ${OPT_PATH}")
    link_directories(${OPT_PATH}/local/lib)
endif(OPT_PATH)


#message(STATUS "SDL2_LIBRARY = ${SDL2_LIBRARY}")
#message(STATUS "SDL2IMAGE_LIBRARY = ${SDL2IMAGE_LIBRARY}")
#message(STATUS SDL2_LINK_DIR = ${SDL2_LINK_DIR})

set(CGENIUS_SRC CGenius.cpp
    misc.cpp
    ${cg_obj_libs})

if(NINTENDO_SWITCH)
    set(CGENIUS_SRC ${CGENIUS_SRC} switch.c)
endif(NINTENDO_SWITCH)

if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    if(ANDROID)
        message(STATUS "Android build enabled. Building CG as Shared Lib")
        add_library(CGeniusExe SHARED ${CGENIUS_SRC})
    else(ANDROID)
        # Add executable Commander Genius
        add_executable (CGeniusExe ${CGENIUS_SRC})
    endif(ANDROID)
else()
    if(ANDROID)
        message(STATUS "Android build enabled. Building CG as Shared Lib")
        add_library(CGeniusExe SHARED)
    else(ANDROID)
        # Add executable Commander Genius
        add_executable (CGeniusExe)
    endif(ANDROID)

    target_sources(CGeniusExe PRIVATE ${CGENIUS_SRC})
endif()

# Since some RefKeen is used, we also need C99 Standard
set_property(TARGET CGeniusExe PROPERTY C_STANDARD 99)

# Link CG Engines
target_link_libraries (CGeniusExe engine)

# Link GsKit
target_link_libraries (CGeniusExe GsKit)


# The Windows Version has an icon in the executable
IF(WIN32)
        SET_TARGET_PROPERTIES(CGeniusExe PROPERTIES OUTPUT_NAME "CGenius")
        find_program(WINDRES_FOUND ${CMAKE_RC_COMPILER})
        if(WINDRES_FOUND)
            ADD_CUSTOM_COMMAND(TARGET CGeniusExe PRE_LINK COMMAND ${WINDRES_FOUND} ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.rc ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o)
            SET_TARGET_PROPERTIES(CGeniusExe PROPERTIES LINK_FLAGS "-mwindows ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o")
            MESSAGE(STATUS "Will build with the CG icon for Windows." )
        else()
            SET_TARGET_PROPERTIES(CGeniusExe PROPERTIES LINK_FLAGS "-mwindows")
        endif(WINDRES_FOUND)
ENDIF(WIN32)



# Only for SDL2 and Win32 since it is updated for that platform.
if(WIN32)
        target_link_libraries(CGeniusExe mingw32)
        target_link_libraries(CGeniusExe user32)
        target_link_libraries(CGeniusExe gdi32)
        target_link_libraries(CGeniusExe winmm)
        target_link_libraries(CGeniusExe imm32)
        target_link_libraries(CGeniusExe version)
endif(WIN32)

# Stuff definitions in case we want to install it
INCLUDE(install.cmake)
