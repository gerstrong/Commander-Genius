
include_directories(${SDL_INCLUDE_DIR})

set(CXXSRC
    CDlgFrame.cpp
    CDlgOptionText.cpp
    CMessageBox.cpp
    CTextViewer.cpp)

set(HSRC
    CDlgFrame.h
    CDlgOptionText.h
    CMessageBox.h
    CTextViewer.h)

add_library(cg_dialog STATIC ${CXXSRC} ${HSRC})

target_link_libraries(cg_dialog GsKit_base_interface)
