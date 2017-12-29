
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

add_library(cg_dialog ${CXXSRC} ${HSRC})


