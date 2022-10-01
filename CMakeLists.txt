cmake_minimum_required(VERSION 3.16)

SET(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics")    # Disable telemetry for vcpkg.
SET(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)             # Install vcpkg dependencies automatically(experimental - might be changed or removed later; see: https://github.com/microsoft/vcpkg/issues/1653). 

# Forbid in-source builds.
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	MESSAGE(SEND_ERROR "In-source builds are not allowed. Use a different build directory.")
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# builds the crossforge library
include("CForge/CrossForgeBuild.cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# create required directories if non existent 
file(MAKE_DIRECTORY "MyAssets")
file(MAKE_DIRECTORY "Prototypes/Shader")

#################################################################
########### Project - CrossForge Sandbox ########################
#################################################################
project (CForgeSandbox)

# Additional packages not alreday included in the core library
FIND_PACKAGE(tinyxml2 CONFIG REQUIRED)	# support for xml loading, storing, and processing

# enable OpenCV support
if(USE_OPENCV)
	FIND_PACKAGE(OpenCV CONFIG REQUIRED)	# Open computer vision library
	include_directories(
		"${OpenCV_INCLUDE_DIRS}"
	)
	add_compile_definitions(USE_OPENCV)
endif(USE_OPENCV)

add_executable(CForgeSandbox 
	SandboxMain.cpp 

	CForge/Core/SGPIO.cpp
	Prototypes/Hardware/I2C.cpp 
	Prototypes/Hardware/MPU6050.cpp
	Prototypes/Hardware/RotaryEncoder.cpp

	Prototypes/Camera/PhotoFinish.cpp
	Prototypes/Camera/StripPhoto.cpp 
	Prototypes/Camera/StripPhotoCamera.cpp

	Prototypes/Misc/IMUCameraController.cpp

	Prototypes/Assets/GLTFIO.cpp

	Prototypes/MeshProcessing/SurfaceSampler.cpp

	Prototypes/Actor/LODActor.cpp
	Prototypes/SLOD.cpp
	Prototypes/LODHandler.cpp
	Prototypes/UBOInstancedData.cpp
	Prototypes/MeshDecimate.cpp
	)


if(WIN32)
	target_link_libraries(CForgeSandbox 
	PRIVATE crossforge
	PRIVATE glfw 
	PRIVATE glad::glad
	ws2_32
	PRIVATE tinyxml2::tinyxml2
#	${FREETYPE_LIBRARIES}

	)
endif (WIN32)

if(UNIX)
	target_link_libraries(CForgeSandbox 
		PRIVATE crossforge
		PRIVATE glfw 
		PRIVATE glad::glad
		PRIVATE tinyxml2::tinyxml2
#		PRIVATE assimp
		PRIVATE gpiod
		PRIVATE dl	
		)
endif(UNIX)

#copy shaders
add_custom_command(
	TARGET CForgeSandbox PRE_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/CForge/Shader
	${CMAKE_CURRENT_BINARY_DIR}/Shader
)
add_custom_command(
	TARGET CForgeSandbox PRE_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
	${CMAKE_SOURCE_DIR}/Prototypes/Shader 
	${CMAKE_CURRENT_BINARY_DIR}/Shader
)

#copy assets
add_custom_command(
	TARGET CForgeSandbox PRE_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
	${CMAKE_SOURCE_DIR}/Assets
	${CMAKE_CURRENT_BINARY_DIR}/Assets
)

add_custom_command(
	TARGET CForgeSandbox PRE_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
	${CMAKE_SOURCE_DIR}/MyAssets
	${CMAKE_CURRENT_BINARY_DIR}/MyAssets
)

######## Project - Export library ######################
project(ExportLibrary)
add_executable(ExportLibrary ExportLibrary.cpp)

target_link_libraries(ExportLibrary PRIVATE crossforge)

include_directories(
	"./"
)

######## Project - Terrain ##############################

project (Terrain)
add_executable(Terrain
	Main.cpp

	# Decoration
	Terrain/src/Decoration/InstanceActor.cpp
	Terrain/src/Decoration/InstanceSGN.cpp
	Terrain/src/Decoration/InstanceVertexUtility.cpp
	
	Prototypes/UBOInstancedData.cpp

	Terrain/src/Map/ClipMap.cpp
	Terrain/src/Map/TileNode.cpp
	Terrain/src/Map/HeightMap.cpp
	Terrain/src/Map/TerrainMap.cpp
	Terrain/src/Map/TileActor.cpp
	Terrain/src/ArrayTexture.cpp

	Terrain/src/Decoration/TreeGenerator.cpp
	Terrain/src/Decoration/RockGenerator.cpp
	Terrain/src/Decoration/DecoSetup.cpp
	Terrain/src/PPScreenQuad.cpp
	
	Prototypes/SLOD.cpp
	"Prototypes/Actor/LODActor.cpp"
	"Prototypes/MeshDecimate.cpp"
	"Prototypes/LODHandler.cpp"
)

if(WIN32)
target_link_libraries(Terrain
	PRIVATE crossforge
	PRIVATE glfw 
	PRIVATE glad::glad
	${FREETYPE_LIBRARIES}
	PRIVATE tinyxml2::tinyxml2
)
endif(WIN32)

if(UNIX)
target_link_libraries(Terrain
	PRIVATE crossforge
	PRIVATE glfw 
	${FREETYPE_LIBRARIES}
	PRIVATE tinyxml2::tinyxml2
)
endif(UNIX)

# copy engine shaders
add_custom_command(
	TARGET Terrain PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/CForge/Shader
	${CMAKE_CURRENT_BINARY_DIR}/Shader
)

#copy prototype shaders
add_custom_command(
	TARGET Terrain PRE_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
	${CMAKE_SOURCE_DIR}/Prototypes/Shader 
	${CMAKE_CURRENT_BINARY_DIR}/Shader
)

#Todo: copy on run
# copy terrain shaders
add_custom_command(
	TARGET Terrain PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/Terrain/Shader
	${CMAKE_CURRENT_BINARY_DIR}/Shader
)

add_custom_command(
	TARGET Terrain PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/Terrain/Assets
	${CMAKE_CURRENT_BINARY_DIR}/Assets
)
