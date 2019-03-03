
include_directories(${SDL_INCLUDE_DIR})


set(SRCS 
BorderedButton.cpp
BorderedButton.h
Button.cpp
Button.h
ComboSelection.cpp
ComboSelection.h
InputText.cpp
InputText.h
NumberControl.cpp
NumberControl.h
Switch.cpp
Switch.h
)

add_library(engine_core_menu_widgets ${SRCS})

# TODO: Not sure why Windows requires this yet.
#if(WIN32)
#    Target_link_libraries(engine_core SDL2)
#endif(WIN32)

Target_link_libraries(engine_core_menu_widgets GsKit)
