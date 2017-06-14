cmake_minimum_required(VERSION 3.7)

project(InstrumentControl)

find_package(OpenCV REQUIRED core)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5SerialPort REQUIRED)

option(USE_THORLABS_APT_CONTROLLER OFF)

set(CMAKE_AUTOMOC ON)

set(SOURCE
   AbstractArduinoDevice.cpp
   ArduinoCounter.cpp
   SerialDevice.cpp
   GenericNewportController.cpp
   
   ImageSource.cpp
   ThreadedObject.cpp
   AbstractImageWriter.cpp
)

set(HEADERS
   AbstractArduinoDevice.h
   ArduinoCounter.h
   PMTStatusWidget.h
   SerialDevice.h
   GenericNewportController.h
   NewportSMC100.h
   NewportNSC200.h
   ThreadedObject.h
   ImageSource.h
   ParametricImageSource.h
   AbstractImageWriter.h
)

if(USE_THORLABS_APT_CONTROLLER)
   set(SOURCE ${SOURCE} ThorlabsAPTController.cpp)
   set(HEADERS ${HEADERS} FTD2XX.h ThorlabsAPTController.h)
   set(APT_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/ftd2xx.lib)
endif()

# manually add include directories since we're not linking libraries in
# this module
include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5SerialPort_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

add_library(InstrumentControl OBJECT ${SOURCE} ${HEADERS} ${UI_HEADERS})

set(InstrumentControl_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(InstrumentControl_LIBRARIES ${APT_LIBS} ${OpenCV_LIBS} PARENT_SCOPE)