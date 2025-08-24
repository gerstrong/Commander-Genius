

Project(GsKit_widgets)

set(HEADERS down.h            GsComboSelection.h  GsMiniTextViewer.h  GsSelectableText.h
    GsBanner.h        GsControl.h         GsNumberControl.h   GsText.h
    GsBaseMenu.h      GsDialog.h          GsProgressbar.h     GsTextSelectionList.h
    GsSelectionList.h
    GsBitmapBox.h     GsFrame.h           GsRegister.h        GsWidget.h
    GsBitmapButton.h  GsInputText.h       GsScrollbar.h       GsWidgetsManager.h
    GsButton.h        GsMenuController.h  GsScrollingFrame.h  up.h
    GsSelectableChkboxItem.h GsCheckbox.h)

set(SRCS GsBanner.cpp          GsFrame.cpp           GsScrollingFrame.cpp
    GsBaseMenu.cpp        GsInputText.cpp       GsSelectableText.cpp
    GsBitmapBox.cpp       GsMenuController.cpp  GsText.cpp
    GsBitmapButton.cpp    GsMiniTextViewer.cpp  GsTextSelectionList.cpp
    GsSelectionList.cpp
    GsButton.cpp          GsNumberControl.cpp   GsWidget.cpp
    GsComboSelection.cpp  GsProgressbar.cpp     GsWidgetsManager.cpp
    GsControl.cpp         GsRegister.cpp
    GsDialog.cpp          GsScrollbar.cpp
    GsSelectableChkboxItem.cpp GsCheckbox.cpp)

add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})

target_link_libraries(${PROJECT_NAME} GsKit_graphics)
