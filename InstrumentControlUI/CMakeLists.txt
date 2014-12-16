cmake_minimum_required(VERSION 2.8.10)

project(InstrumentControlUI)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)

find_package(OpenCV REQUIRED)

find_package(Qt5Widgets REQUIRED)

qt5_wrap_ui(UI_HEADERS ImageRenderWindow.ui)

#qt5_add_resources(UI_RESOURCES PuppetMasterResources.qrc)

set(CMAKE_AUTOMOC ON)

set(SOURCE
   ImageRenderWidget.cpp
)


set(HEADERS
   ControlBinder.h
   ImageRenderWidget.h
   ImageRenderWindow.h
)

# manually add include directories since we're not linking libraries in
# this module
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS} ${Qt5SerialPort_INCLUDE_DIRS} ${InstrumentControl_INCLUDE_DIR})

set(InstrumentControlUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${InstrumentControl_INCLUDE_DIR} PARENT_SCOPE)

add_library(InstrumentControlUI OBJECT ${SOURCE} ${HEADERS} ${UI_HEADERS})


