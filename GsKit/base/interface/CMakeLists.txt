Project(GsKit_base_interface)

set(HEADERS Base64.h         Event.h       MathLib.h        Ref.h          Utils.h
Color.h          EventQueue.h  misc.h           StringBuf.h
ConfigHandler.h  FindFile.h    Mutex.h          StringUtils.h
CrashHandler.h   Geometry.h    ReadWriteLock.h  ThreadPool.h
Debug.h          Iterator.h    RefCounter.h     Unicode.h )

set(SRCS Base64.cpp         CrashHandler.cpp  Geometry.cpp     ThreadPool.cpp
Color.cpp          Debug.cpp         StringBuf.cpp    Unicode.cpp
ConfigHandler.cpp  FindFile.cpp      StringUtils.cpp  MathLib.cpp)

set(HEADERS ${HEADERS}
    property_tree/property_tree.h
    property_tree/xml_parser.h)

set(SRCS ${SRCS}
    property_tree/property_tree.cpp
    property_tree/xml_parser.cpp)

IF (NOT Boost_FOUND OR USE_BOOST)
        set(HEADERS ${HEADERS} property_tree/tinyxml2.h)
        set(SRCS ${SRCS} property_tree/tinyxml2.cpp)
endif(NOT Boost_FOUND OR USE_BOOST)

add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})

option(USE_SDL3 "Build using SDL3 instead of SDL2" OFF)

if(USE_SDL3)
    find_package(SDL3 REQUIRED)
    find_package(SDL3_image REQUIRED)
    find_package(SDL3_mixer REQUIRED)
    find_package(SDL3_ttf REQUIRED)

    target_link_libraries(${PROJECT_NAME} PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer
        SDL3_ttf::SDL3_ttf
    )
    message(STATUS "Configured with SDL3")
else()
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)

    if(SDL2_FOUND)
        # Prefer SDL2 modern targets if available, otherwise fallback to variables
        if(TARGET SDL2::SDL2)
            target_link_libraries(${PROJECT_NAME} PUBLIC
                SDL2::SDL2
                SDL2_image::SDL2_image
                SDL2_mixer::SDL2_mixer
                SDL2_ttf::SDL2_ttf
            )
        else()
            target_include_directories(${PROJECT_NAME} PUBLIC
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${SDL2_INCLUDE_DIRS}
                ${SDL2_IMAGE_INCLUDE_DIRS}
                ${SDL2_MIXER_INCLUDE_DIRS}
                ${SDL2_TTF_INCLUDE_DIRS}
            )
            target_link_libraries(${PROJECT_NAME} PUBLIC
                ${SDL2_LIBRARIES}
                ${SDL2_IMAGE_LIBRARIES}
                ${SDL2_MIXER_LIBRARIES}
                ${SDL2_TTF_LIBRARIES}
            )
        endif()

        message(STATUS "Configured with SDL2")
    else()
        message(FATAL_ERROR "SDL2 was not detected!")
    endif()
endif()

