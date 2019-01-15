cmake_minimum_required(VERSION 3.7)

project(InstrumentControl)

find_package(OpenCV REQUIRED core)
find_package(Qt5 REQUIRED COMPONENTS Widgets SerialPort)

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


add_library(InstrumentControl STATIC ${SOURCE} ${HEADERS} ${UI_HEADERS})

target_include_directories(InstrumentControl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                             PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(InstrumentControl PUBLIC ${APT_LIBS} ${OpenCV_LIBS} Qt5::Widgets Qt5::SerialPort)
