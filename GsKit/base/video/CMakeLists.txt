cmake_minimum_required(VERSION 3.7)

Project(GsKit_base_video)


set(HEADERS CEffects.h  CSDLVideo.h   CVideoDriver.h  GsEffectController.h
COpenGL.h   CVidConfig.h  CVideoEngine.h
scaler/CScaler.h  scaler/scale2x.h  scaler/scale3x.h  scaler/scalebit.h)

set(SRCS COpenGL.cpp  CSDLVideo.cpp  CVidConfig.cpp  CVideoDriver.cpp  CVideoEngine.cpp
scaler/CScaler.cpp  scaler/scale2x.cpp  scaler/scale3x.cpp  scaler/scalebit.cpp)

add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})

target_link_libraries(${PROJECT_NAME} GsKit_graphics)
