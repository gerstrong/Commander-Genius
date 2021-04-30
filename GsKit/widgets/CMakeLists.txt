cmake_minimum_required(VERSION 3.7)

Project(GsKit_widgets)

set(HEADERS down.h            GsComboSelection.h  GsMiniTextViewer.h  GsSelectableText.h
    GsBanner.h        GsControl.h         GsNumberControl.h   GsText.h
    GsBaseMenu.h      GsDialog.h          GsProgressbar.h     GsTextSelectionList.h
    GsBitmapBox.h     GsFrame.h           GsRegister.h        GsWidget.h
    GsBitmapButton.h  GsInputText.h       GsScrollbar.h       GsWidgetsManager.h
    GsButton.h        GsMenuController.h  GsScrollingFrame.h  up.h)

set(SRCS GsBanner.cpp          GsFrame.cpp           GsScrollingFrame.cpp
    GsBaseMenu.cpp        GsInputText.cpp       GsSelectableText.cpp
    GsBitmapBox.cpp       GsMenuController.cpp  GsText.cpp
    GsBitmapButton.cpp    GsMiniTextViewer.cpp  GsTextSelectionList.cpp
    GsButton.cpp          GsNumberControl.cpp   GsWidget.cpp
    GsComboSelection.cpp  GsProgressbar.cpp     GsWidgetsManager.cpp
    GsControl.cpp         GsRegister.cpp
    GsDialog.cpp          GsScrollbar.cpp)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})

target_link_libraries(${PROJECT_NAME} GsKit_graphics)
