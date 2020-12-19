
include_directories(${SDL_INCLUDE_DIR})


set(SRCS 
button1.h
button2.h
button3.h
button4.h
buttonClose.h
buttonConfirm.h
buttonStart.h
dpad.h
vgamepadsimple.cpp
vgamepadsimple.h
)

add_library(engine_core_vgamepad STATIC ${SRCS})

Target_link_libraries(engine_core_vgamepad GsKit)



