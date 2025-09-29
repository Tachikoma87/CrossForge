cmake_minimum_required(VERSION 3.16)

SET(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics")    # Disable telemetry for vcpkg.
SET(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)             # Install vcpkg dependencies automatically(experimental - might be changed or removed later; see: https://github.com/microsoft/vcpkg/issues/1653). 

# Forbid in-source builds.
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	MESSAGE(SEND_ERROR "In-source builds are not allowed. Use a different build directory.")
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# builds the crossforge library
#include("crossforge/CrossForgeBuild.cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# create required directories if non existent 
file(MAKE_DIRECTORY "MyAssets")
file(MAKE_DIRECTORY "Prototypes")
file(MAKE_DIRECTORY "Prototypes/Shader")

#################################################################
########### Project - CrossForge Sandbox ########################
#################################################################
project (CForgeSandbox)

option(INCLUDE_OPENCV "Include OpenCV in build" OFF)
include(FetchContent)

FetchContent_Declare(
	libcrossforge
	SOURCE_DIR "C:/Projects/libcrossforge"
)
FetchContent_MakeAvailable(libcrossforge)
include_directories(${libcrossforge_SOURCE_DIR}/include)



if(EMSCRIPTEN)
	
	
	### Eigen3
	FetchContent_Declare(
		tinyxml2 
		GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
		GIT_TAG 9.0.0
	)
	FetchContent_MakeAvailable(tinyxml2)

	# tinygltf
	FetchContent_Declare(
		tinygltf 
		URL https://github.com/syoyo/tinygltf/archive/refs/tags/v2.8.2.zip
	)
	FetchContent_MakeAvailable(tinygltf)
	include_directories(${tinygltf_SOURCE_DIR}/)

	FetchContent_Declare(
		imgui
		URL https://github.com/ocornut/imgui/archive/refs/tags/v1.89.4.zip
	)
	FetchContent_MakeAvailable(imgui)
	include_directories(${imgui_SOURCE_DIR}/)
	include_directories(${imgui_SOURCE_DIR}/backends)


	set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
# Additional packages not alreday included in the core library
FIND_PACKAGE(tinyxml2 CONFIG REQUIRED)	# support for xml loading, storing, and processing
FIND_PACKAGE(imgui CONFIG REQUIRED) # dear-imgui
FIND_PACKAGE(JPEG REQUIRED)
FIND_PACKAGE(libjpeg-turbo)
FIND_PACKAGE(FFMPEG REQUIRED)
FIND_PACKAGE(assimp REQUIRED)
FIND_PACKAGE(glad CONFIG REQUIRED)	# GL extension library
FIND_PACKAGE(glfw3 CONFIG REQUIRED)	# cross-plattform window management
FIND_PACKAGE(Eigen3 CONFIG REQUIRED)
endif()

add_executable(CForgeSandbox 
	

	# SandboxMain.cpp
	

	# Hardware stuff (used with Raspberry Pi)
#	crossforge/Core/SGPIO.cpp
#	Prototypes/Hardware/I2C.cpp 
#	Prototypes/Hardware/MPU6050.cpp
#	Prototypes/Hardware/RotaryEncoder.cpp

	# Camera related
#	Prototypes/Camera/PhotoFinish.cpp
#	Prototypes/Camera/StripPhoto.cpp 
#	Prototypes/Camera/VideoPlayer.cpp
#	Prototypes/Camera/CameraCapture.cpp
#	Prototypes/Multimedia/SMediaDeviceManager.cpp
#	Prototypes/Multimedia/CameraDevice.cpp
#	Prototypes/Multimedia/FFMPEG.cpp
	
	# Misc
#	Prototypes/Assets/GLTFIO.cpp
#	Prototypes/MeshProcessing/SurfaceSampler.cpp
	
	
#	"Prototypes/GUI/ImGuiUtility.cpp"
	

	# Subprojects
	# B02 Demonstrator
#	Subprojects/B02Demonstrator/AdaptiveSkeletalActor.cpp
	# Dual IMU
#	Subprojects/DualIMU/IMUCameraController.cpp

	# Other
#	Prototypes/Actor/LODActor.cpp
#	"Prototypes/Actor/SLOD.cpp"
#	"Prototypes/Actor/LODHandler.cpp"
#	"Prototypes/Actor/UBOInstancedData.cpp"
#	"Prototypes/MeshProcessing/MeshDecimate.cpp"

	# ECS
#	"Prototypes/ECS/ComponentBase.cpp"
#	"Prototypes/ECS/EntityBase.cpp"
#	"Prototypes/ECS/SystemBase.cpp"
#	"Prototypes/ECS/SSystemManager.cpp"
#	"Prototypes/ECS/SEntityManager.cpp"

#	"Prototypes/ECS/PrototypeComponents/PositionComponent2D.cpp"
#	"Prototypes/ECS/PrototypeComponents/MovementSystem.cpp"

	# AssetIO ECS
#	"Prototypes/AssetIO/Components/MaterialDataComponent.cpp"
#	"Prototypes/AssetIO/Components/NormalDataComponent.cpp"
#	"Prototypes/AssetIO/Components/PositionDataComponent.cpp"
#	"Prototypes/AssetIO/Components/MeshDefinitionsComponen.cpp"
#	"Prototypes/AssetIO/Components/TextureCoordinatesComponent.cpp"

#	"Prototypes/AssetIO/Entities/FileEntity.cpp"
#	"Prototypes/AssetIO/Entities/Image2DEntity.cpp"
#	"Prototypes/AssetIO/Entities/TriangleMeshEntity.cpp"

#	"Prototypes/AssetIO/Managers/SAssetIOManager.cpp"
#
#	"Prototypes/AssetIO/Systems/TriangleMeshIOSystemBase.cpp"
#	"Prototypes/AssetIO/Systems/AssimpMeshIOSystem.cpp"

#	"Prototypes/AssetIO/TriangleMesh/MeshDefinition.cpp"
#	"Prototypes/AssetIO/TriangleMesh/MeshMaterial.cpp"

	
#	"Prototypes/Graphics/Components/WindowPropertiesComponent.cpp"
#	"Prototypes/Graphics/Entities/WindowEntity.cpp"
#	"Prototypes/Graphics/Systems/RenderingSystem.cpp"
#	"Prototypes/Graphics/Systems/WindowSystem.cpp"
	
	################################
	#### New CrossForge ############
	################################

	 "CrossForgeSandbox.cpp" 
	  
	 "Prototypes/applications/SCrossForgeTestApp.cpp"

	"Prototypes/Graphics/Systems/RenderingSystem.cpp"
	"Prototypes/ECS/PrototypeComponents/PositionComponent2D.cpp"

    )
	
if(EMSCRIPTEN)
	target_link_Libraries(CForgeSandbox
		tinyxml2
		crossforge
	)
	set_target_properties(CForgeSandbox PROPERTIES LINK_FLAGS "${LINK_FLAGS} ${Optimization_Flag} -sEXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1 -sWASM=1 -sUSE_WEBGL2=1 -fwasm-exceptions -sUSE_GLFW=3 -sUSE_ZLIB=1 -sUSE_LIBPNG=1 -sUSE_LIBJPEG=1 --preload-file Assets --preload-file Shader --preload-file MyAssets")

elseif(WIN32)
#	target_include_directories(CForgeSandbox 
#	PRIVATE ${JPEG_INCLUDE_DIR}
#	PRIVATE ${FFMPEG_INCLUDE_DIRS}
#	)

	target_link_directories(CForgeSandbox
		PRIVATE ${FFMPEG_LIBRARY_DIRS}
	)




	target_link_libraries(CForgeSandbox 
	#PRIVATE crossforge
	PRIVATE crossforge
	PRIVATE Eigen3::Eigen
	PRIVATE glfw 
	PRIVATE glad::glad
#	ws2_32
#	PRIVATE tinyxml2::tinyxml2
#	PRIVATE imgui::imgui
#	PRIVATE ${FFMPEG_LIBRARIES}
#	${FREETYPE_LIBRARIES}
#	PRIVATE assimp::assimp
	
	)
	
elseif(__arm__)
	target_link_libraries(CForgeSandbox 
		PRIVATE crossforge
		PRIVATE glfw
		PRIVATE glad::glad
		PRIVATE tinyxml2::tinyxml2
		PRIVATE imgui::imgui
#		PRIVATE assimp
		PRIVATE gpiod
		PRIVATE dl	
		)
elseif(UNIX)
	target_include_directories(CForgeSandbox 
	PRIVATE ${JPEG_INCLUDE_DIR}
	PRIVATE ${FFMPEG_INCLUDE_DIRS}
	)

	target_link_directories(CForgeSandbox
		PRIVATE ${FFMPEG_LIBRARY_DIRS}
	)

	target_link_libraries(CForgeSandbox 
		PRIVATE crossforge
		PRIVATE glfw
		PRIVATE glad::glad
		PRIVATE tinyxml2::tinyxml2
		PRIVATE imgui::imgui
		PRIVATE dl	
		PRIVATE ${FFMPEG_LIBRARIES}
		${FREETYPE_LIBRARIES}
		${OpenCV_LIBS}
		)
endif()

#copy shaders
#add_custom_command(
#	TARGET CForgeSandbox PRE_BUILD 
#	COMMAND ${CMAKE_COMMAND} -E copy_directory
#	${CMAKE_SOURCE_DIR}/crossforge/Shader
#	${CMAKE_CURRENT_BINARY_DIR}/Shader
#)
#add_custom_command(
#	TARGET CForgeSandbox PRE_BUILD 
#	COMMAND ${CMAKE_COMMAND} -E copy_directory 
#	${CMAKE_SOURCE_DIR}/Prototypes/Shader 
#	${CMAKE_CURRENT_BINARY_DIR}/Shader
#

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


if(EMSCRIPTEN)
set(APP_FILES CForgeSandbox.html CForgeSandbox.js CForgeSandbox.wasm CForgeSandbox.data)
foreach(i ${APP_FILES})
add_custom_command(
	TARGET CForgeSandbox POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy 
	${CMAKE_CURRENT_BINARY_DIR}/${i}
	${CMAKE_CURRENT_BINARY_DIR}/App/${i}
)
endforeach(i)
endif(EMSCRIPTEN)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)