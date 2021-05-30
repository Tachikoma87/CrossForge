cmake_minimum_required(VERSION 3.8)

include_directories(
	Libraries/eigen-master
	Libraries/opencv-4.5.1/include)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# CForge Core library
project(crossforgecore VERSION 0.1)
add_library(crossforgecore STATIC 
	Core/SCrossForgeDevice.cpp
	Core/SLogger.cpp
	Core/CrossForgeException.cpp
	Core/File.cpp)
set_target_properties(crossforgecore PROPERTIES VERSION ${PROJECT_VERSION})

project (CForgeSandbox)
add_executable(CForgeSandbox 
	SandboxMain.cpp 

	Prototypes/SGPIO.cpp 
	Prototypes/I2C.cpp 
	Prototypes/MPU6050.cpp
	Prototypes/RotaryEncoder.cpp
	Prototypes/PhotoFinish.cpp
	Prototypes/StripPhoto.cpp 
	Prototypes/StripPhotoCamera.cpp)

if(WIN32)
	target_link_libraries(CForgeSandbox crossforgecore 
	${PROJECT_SOURCE_DIR}/Libraries/opencv-4.5.1/x64/vc16/lib/opencv_core451.lib
	${PROJECT_SOURCE_DIR}/Libraries/opencv-4.5.1/x64/vc16/lib/opencv_video451.lib
	${PROJECT_SOURCE_DIR}/Libraries/opencv-4.5.1/x64/vc16/lib/opencv_videoio451.lib
	${PROJECT_SOURCE_DIR}/Libraries/opencv-4.5.1/x64/vc16/lib/opencv_highgui451.lib)
endif (WIN32)

if(UNIX)
	target_link_libraries(crossforgecore gpiod stdc++fs)
	target_link_libraries(CForgeSandbox crossforgecore)
endif(UNIX)

