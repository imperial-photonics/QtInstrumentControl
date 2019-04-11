cmake_minimum_required(VERSION 3.7)

project(InstrumentControlUI)

find_package(OpenCV REQUIRED core highgui)

find_package(Qt5 COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
cmake_policy(SET CMP0071 OLD)


qt5_wrap_ui(UI_HEADERS ImageRenderWindow.ui
                       ImageSeriesControl.ui
                       TaskProgressWidget.ui)

set(SOURCE
   ImageRenderWidget.cpp
   ParameterWidget.cpp
   TaskProgress.cpp
   CustomDialog.cpp
   QLedIndicator.cpp
)

set(HEADERS
   ControlBinder.h
   BoundControl.h
   BoundControlImpl.h
   BoundPropertyControl.h
   BoundPropertyControlImpl.h
   ImageRenderWidget.h
   ImageRenderWindow.h
   ImageSeriesControl.h
   ImageSeriesScanner.h
   ParameterWidget.h
   ConstrainedMdiSubWindow.h
   TaskProgress.h
   TaskProgressWidget.h
   CustomDialog.h
   QLedIndicator.h
)

add_library(InstrumentControlUI STATIC ${SOURCE} ${HEADERS} ${UI_HEADERS})

target_include_directories(InstrumentControlUI PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
                                                       ${CMAKE_CURRENT_BINARY_DIR} 
                                               PRIVATE ${OpenCV_INCLUDE_DIRS}
                                                       Qt5::Widgets)

target_link_libraries(InstrumentControlUI InstrumentControl)
