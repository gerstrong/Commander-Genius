
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
KeenStyles.h
)

add_library(engine_core_menu_widgets STATIC ${SRCS})

Target_link_libraries(engine_core_menu_widgets GsKit)
