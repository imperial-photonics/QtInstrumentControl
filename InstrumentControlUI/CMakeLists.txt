cmake_minimum_required(VERSION 3.7)

project(InstrumentControlUI)

find_package(OpenCV REQUIRED core highgui)

find_package(Qt5Widgets REQUIRED)

qt5_wrap_ui(UI_HEADERS ImageRenderWindow.ui
                       ImageSeriesControl.ui
                       TaskProgressWidget.ui)

set(CMAKE_AUTOMOC ON)

set(SOURCE
   ImageRenderWidget.cpp
   ParameterWidget.cpp
   TaskProgress.cpp
)


set(HEADERS
   ControlBinder.h
   ImageRenderWidget.h
   ImageRenderWindow.h
   ImageSeriesControl.h
   ImageSeriesScanner.h
   ParameterWidget.h
   ConstrainedMdiSubWindow.h
   TaskProgress.h
   TaskProgressWidget.h
)

# manually add include directories since we're not linking libraries in this module
set(COMMON_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} ${InstrumentControl_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${OpenCV_INCLUDE_DIRS})
include_directories(${COMMON_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5SerialPort_INCLUDE_DIRS})

set(InstrumentControlUI_INCLUDE_DIR ${COMMON_INCLUDE} PARENT_SCOPE)

add_library(InstrumentControlUI OBJECT ${SOURCE} ${HEADERS} ${UI_HEADERS})


