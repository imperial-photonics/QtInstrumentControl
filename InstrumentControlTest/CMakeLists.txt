cmake_minimum_required(VERSION 3.0)

project(InstrumentControlTest)

find_package(OpenCV REQUIRED)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5Declarative REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Designer REQUIRED)

set(CMAKE_AUTOMOC ON)
cmake_policy(SET CMP0071 OLD)

include_directories(${COMMON_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT_CUSTOM_WIDGETS_INCLUDE_DIR})

qt5_wrap_ui(UI_HEADERS ArduinoCounterTest.ui)

add_executable(InstrumentControlTest ArduinoCounterTest.h ArduinoCounterTest.cpp ${UI_HEADERS})

qt5_use_modules(InstrumentControlTest Widgets PrintSupport OpenGL Gui SerialPort)

set_target_properties(InstrumentControlTest PROPERTIES LINK_FLAGS "/NODEFAULTLIB:library")

target_link_libraries(InstrumentControlTest
                      QTDesignerPlugins
                      Qt5::Widgets 
                      Qt5::PrintSupport
                      Qt5::OpenGL 
                      Qt5::Gui
                      Qt5::SerialPort
                      InstrumentControl
                      InstrumentControlUI)