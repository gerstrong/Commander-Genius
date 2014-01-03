# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 2.8)

project(GsKit_base)

# TODO: From this point we should recursively glob the files. I think it is the easiest way...

add_library(GsKit_base GsApp.cpp
                       GsAppState.cpp
                       GsLogging.cpp
                       GsTimer.cpp
                       GsEvent.h
                       GsEngine.h
                       CInput.cpp
                       video/CVideoEngine.cpp
                       video/CVideoDriver.cpp
                       video/CSDLVideo.cpp
                       video/COpenGL.cpp
                       video/scaler/CScaler.cpp
                       video/scaler/CScaler.cpp
                       video/scaler/CScaler.cpp
                       video/scaler/scale2x.cpp
                       video/scaler/scale3x.cpp
                       video/scaler/scalebit.cpp)
