cmake_minimum_required(VERSION 3.0)

project(CameraControl) 

#option(INCLUDE_CAMERA_ANDOR "Use Andor Camera" OFF)
#option(INCLUDE_CAMERA_XIMEA "Use Ximea Camera" OFF)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)

find_package(OpenGL REQUIRED)
find_package(CUDA 5.5 QUIET)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Declarative REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5OpenGL REQUIRED)

find_package(OpenCV REQUIRED)

set(CMAKE_AUTOMOC ON)



# Source Files
#====================================
if (INCLUDE_CAMERA_ANDOR)
   set(ANDOR_SOURCE
      AndorCamera.cpp
      AndorCamera.h
      AndorControlDisplay.h
      AndorControlWidget.h
      AndorControlWidget.cpp
      AndorControlDisplay.cpp
   )
   set(ANDOR_LIBRARIES ${ANDOR_DIR}/atcorem.lib)
endif (INCLUDE_CAMERA_ANDOR)
if (INCLUDE_CAMERA_XIMEA)
   set(Ximea_SOURCE
      XimeaCamera.h
      XimeaControlDisplay.h
      XimeaCamera.cpp
      XimeaControlDisplay.cpp
   )
   set(Ximea_LIBRARIES ${Ximea_DIR}/x64/m3apiX64.lib)
endif (INCLUDE_CAMERA_XIMEA)

set(SOURCE
   ImageBuffer.cpp
   AbstractStreamingCamera.cpp
   CameraControlWidget.cpp
   ImageWriter.cpp
)

set(HEADERS
   AbstractStreamingCamera.h
   ImageBuffer.h
   CameraControlWidget.h
   ImageWriter.h
)

include_directories(${Ximea_DIR} ${ANDOR_DIR} ${QT_USE_FILE} ${COMMON_INCLUDE_DIR} ${InstrumentControl_INCLUDE_DIR})

if (CUDA_FOUND)
	include_directories(${CUDA_INCLUDE_DIRS})
   add_definitions(-DUSE_CUDA)
   set(CameraControl_CUDA_LIBRARIES ${CUDA_LIBRARIES} ${CUDA_CUDA_LIBRARIES})
endif (CUDA_FOUND)

# Linking
#====================================

add_library(CameraControl STATIC ${SOURCE} ${HEADERS} ${ANDOR_SOURCE} ${Ximea_SOURCE})

qt5_use_modules(CameraControl Widgets PrintSupport OpenGL Gui)

set(CameraControl_INCLUDE_DIR ${Ximea_DIR}
                              ${ANDOR_DIR} 
                              ${InstrumentControl_INCLUDE_DIR} 
                              ${CMAKE_CURRENT_SOURCE_DIR} 
                              PARENT_SCOPE)

set(CameraControl_LIBRARIES ${Ximea_LIBRARIES}
                            ${ANDOR_LIBRARIES}
                            ${CameraControl_CUDA_LIBRARIES} 
                            ${OpenCV_LIBS} 
                            PARENT_SCOPE)
