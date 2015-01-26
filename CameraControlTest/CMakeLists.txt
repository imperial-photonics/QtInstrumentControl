cmake_minimum_required(VERSION 3.0)

project(CameraControlTest)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5Declarative REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5OpenGL REQUIRED)

find_package(OpenCV REQUIRED)

include_directories(${CameraControl_INCLUDE_DIR})


add_executable(CameraControlTest AndorTest.cpp)

qt5_use_modules(CameraControlTest Widgets PrintSupport OpenGL Gui)

set_target_properties(CameraControlTest PROPERTIES LINK_FLAGS "/NODEFAULTLIB:library" )

target_link_libraries(CameraControlTest ${CameraControl_LIBRARIES} CameraControl)
CUDA_ADD_CUFFT_TO_TARGET( CameraControlTest ) 
   