cmake_minimum_required(VERSION 3.21)

SET(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics")    # Disable telemetry for vcpkg.
SET(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)             # Install vcpkg dependencies automatically(experimental - might be changed or removed later; see: https://github.com/microsoft/vcpkg/issues/1653). 
SET(X_VCPKG_APPINSTALL_DEPS_INSTALL ON)

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

if(UNIX)
FetchContent_Declare(
	libcrossforge
	SOURCE_DIR "/mnt/c/Projects/libcrossforge"
)
else()
FetchContent_Declare(
	libcrossforge
	SOURCE_DIR "C:/Projects/libcrossforge"
)
endif()
FetchContent_MakeAvailable(libcrossforge)


FetchContent_Declare(
	stb 
	GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)
include_directories(${stb_SOURCE_DIR})


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
FIND_PACKAGE(WebP CONFIG REQUIRED)	# WebP to import/export webp
FIND_PACKAGE(tinyfiledialogs CONFIG REQUIRED)

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

	################################
	#### New CrossForge ############
	################################


#
#	"Prototypes/AssetIO/Systems/TriangleMeshIOSystemBase.cpp"
#	"Prototypes/AssetIO/Systems/AssimpMeshIOSystem.cpp"

#	"Prototypes/AssetIO/TriangleMesh/MeshDefinition.cpp"
#	"Prototypes/AssetIO/TriangleMesh/MeshMaterial.cpp"

	
#	"Prototypes/Graphics/Components/WindowPropertiesComponent.cpp"
#	"Prototypes/Graphics/Entities/WindowEntity.cpp"
#	"Prototypes/Graphics/Systems/RenderingSystem.cpp"
#	"Prototypes/Graphics/Systems/WindowSystem.cpp"
	
	
	

	"CrossForgeSandbox.cpp" 
	"Prototypes/applications/SCrossForgeTestApp.cpp"
	"Prototypes/Graphics/controllers/RenderingController.cpp"
   
     "Prototypes/Graphics/Entities/ShaderEntity.h"
	 "Prototypes/Graphics/entities/ShaderEntity.cpp"
	 "Prototypes/Graphics/controllers/ShaderEntityController.h"
	 "Prototypes/Graphics/controllers/ShaderEntityController.cpp"
	 "Prototypes/Graphics/entities/ActorPrefabEntity.h"
	 "Prototypes/Graphics/entities/ActorPrefabEntity.cpp"
	 "Prototypes/Graphics/controllers/ActorPrefabEntityController.h"
	 "Prototypes/Graphics/controllers/ActorPrefabEntityController.cpp"
	 
	 
	 "Prototypes/scene/controllers/LightEntityController.h"
	 "Prototypes/scene/controllers/LightEntityController.cpp"
	 "Prototypes/scene/entities/CameraEntity.h"
	 "Prototypes/scene/entities/CameraEntity.cpp"
	 "Prototypes/scene/entities/LightEntity.cpp"
	 "Prototypes/scene/controllers/CameraEntityController.h"
	 "Prototypes/scene/controllers/CameraEntityController.cpp"
	 "Prototypes/utility/GraphicsUtility.h"
	 "Prototypes/utility/GraphicsUtility.cpp"
	 "Prototypes/Graphics/components/shader/ShaderSourceComponent.h"
	 "Prototypes/Graphics/components/shader/RenderingShaderComponent.h"
	 "Prototypes/Graphics/components/shader/RenderingShaderComponent.cpp"
	 "Prototypes/Graphics/components/shader/ShaderSourceComponent.cpp"
	 "Prototypes/Graphics/components/actorprefab/VertexBufferComponent.h"
	 "Prototypes/Graphics/components/actorprefab/IndexBufferComponent.h"
	 "Prototypes/Graphics/components/actorprefab/VertexBufferComponent.cpp"
	 "Prototypes/Graphics/components/actorprefab/IndexBufferComponent.cpp"
	 "Prototypes/Graphics/components/actorprefab/RenderGroupsComponent.h"
	 "Prototypes/Graphics/components/actorprefab/RenderGroupsComponent.cpp"
	 "Prototypes/Graphics/components/actorprefab/PbrMaterialsComponent.h"
	 "Prototypes/Graphics/components/actorprefab/PbrMaterialsComponent.cpp" 
	 "Prototypes/Graphics/components/actorprefab/VertexArrayComponent.h" 
	 "Prototypes/Graphics/components/actorprefab/VertexArrayComponent.cpp" 
	 "Prototypes/scene/entities/SceneObjectEntity.h" 
	 "Prototypes/scene/entities/SceneObjectEntity.cpp" 
	 "Prototypes/scene/entities/ActorInstanceEntity.h" 
	 "Prototypes/scene/entities/ActorInstanceEntity.cpp" 
	 "Prototypes/scene/components/Transformation3DComponent.h" 
	 "Prototypes/scene/components/Transformation3DComponent.cpp" 
	 "Prototypes/scene/components/Movement3DComponent.h" 
	 "Prototypes/scene/components/Movement3DComponent.cpp" 
	 "Prototypes/Graphics/components/uniformbuffer/UBOCameraDataComponent.h" 
	 "Prototypes/Graphics/components/uniformbuffer/UBOCameraDataComponent.cpp" 
	 "Prototypes/Graphics/components/uniformbuffer/UBOTransformationDataComponent.h" 
	 "Prototypes/Graphics/components/uniformbuffer/UBOTransformationDataComponent.cpp" 
	 "Prototypes/Graphics/components/uniformbuffer/UniformBufferComponent.h" 
	 "Prototypes/Graphics/components/uniformbuffer/UniformBufferComponent.cpp"    
	 "Prototypes/Graphics/components/ColorComponent.h" 
	 "Prototypes/Graphics/components/ColorComponent.cpp" 
	 "Prototypes/applications/SCrossForgeSimpleSceneApp.h" 
	 "Prototypes/applications/SCrossForgeSimpleSceneApp.cpp" 
	 "Prototypes/scene/components/CameraPropertiesComponent.h" 
	 "Prototypes/scene/components/CameraPropertiesComponent.cpp" 
	 "Prototypes/Graphics/entities/TextureEntity.h" 
	 "Prototypes/Graphics/entities/TextureEntity.cpp" 
	 "Prototypes/Graphics/components/Texture2DComponent.h" 
	 "Prototypes/Graphics/components/Texture2DComponent.cpp" 
	 "Prototypes/Graphics/controllers/TextureEntityController.h" 
	 "Prototypes/Graphics/controllers/TextureEntityController.cpp"
	 "Prototypes/Graphics/controllers/Image2DController.h" 
	 "Prototypes/Graphics/controllers/Image2DController.cpp" 

	 "Prototypes/Graphics/components/LightsConfigComponent.cpp"
 "Prototypes/Graphics/components/uniformbuffer/UBODirectionalLightsComponent.h"
 "Prototypes/Graphics/components/uniformbuffer/UBODirectionalLightsComponent.cpp"
 "Prototypes/Graphics/components/uniformbuffer/UBOPbrMaterialComponent.h"
 "Prototypes/Graphics/components/uniformbuffer/UBOPbrMaterialComponent.cpp"
 "Prototypes/Graphics/entities/LightsEntity.h"
 "Prototypes/Graphics/entities/LightsEntity.cpp"
 "Prototypes/Graphics/controllers/TriangleMeshController.h"
 "Prototypes/Graphics/controllers/TriangleMeshController.cpp"
 "Prototypes/Graphics/components/uniformbuffer/UBOPointLightsComponent.h"
 "Prototypes/Graphics/components/uniformbuffer/UBOSpotLightsComponent.h"
 "Prototypes/Graphics/components/uniformbuffer/UBOSpotLightsComponent.cpp"
 "Prototypes/Graphics/components/uniformbuffer/UBOPointLightsComponent.cpp"
 "Prototypes/scene/components/ChildObjectsComponent.h"
 "Prototypes/scene/components/ChildObjectsComponent.cpp"
 "Prototypes/Graphics/provider/STextureProvider.h"
 "Prototypes/Graphics/provider/STextureProvider.cpp"
 "Prototypes/Graphics/provider/SShaderProvider.h"
 "Prototypes/Graphics/provider/SShaderProvider.cpp"
 "Prototypes/scene/controllers/SceneObjectEntityController.h"
 "Prototypes/scene/controllers/SceneObjectEntityController.cpp"
 "Prototypes/scene/components/PrefabComponent.h"
 "Prototypes/scene/components/PrefabComponent.cpp"
 "Prototypes/scene/controllers/ActorInstanceEntityController.h"
 "Prototypes/scene/controllers/ActorInstanceEntityController.cpp"
 "Prototypes/scene/systems/MovementSystem.h"
 "Prototypes/scene/systems/MovementSystem.cpp"
 "Prototypes/Graphics/components/shader/ShaderPropertiesComponent.h"
 "Prototypes/Graphics/components/shader/ShaderPropertiesComponent.cpp"
 "Prototypes/utility/MiscUtility.hpp" "Prototypes/scene/entities/SceneEntity.h"
 "Prototypes/scene/entities/SceneEntity.cpp"
 "Prototypes/Graphics/components/actorprefab/ActorPrefabPropertiesComponent.h"
 "Prototypes/Graphics/components/actorprefab/ActorPrefabPropertiesComponent.cpp"
 "Prototypes/utility/MiscUtility.cpp" 
 "Prototypes/input/controllers/InputDeviceController.h"
 "Prototypes/input/controllers/InputDeviceController.cpp"
 
 "Prototypes/input/components/UserDialogComponent.h"
 "Prototypes/input/components/UserDialogComponent.cpp"
 "Prototypes/input/components/UserInputComponent.h"
 "Prototypes/input/components/UserInputComponent.cpp"
 "Prototypes/utility/FFMpegUtility.hpp"
 "Prototypes/assetio/entities/VideoEntity.h" "Prototypes/assetio/entities/VideoEntity.cpp" "Prototypes/assetio/components/VideoDataComponent.h" "Prototypes/assetio/components/VideoDataComponent.cpp" "Prototypes/assetio/controllers/VideoController.h" "Prototypes/assetio/controllers/VideoController.cpp" "Prototypes/scene/components/SphericalTransformationComponent.h" "Prototypes/scene/components/SphericalTransformationComponent.cpp" "Prototypes/scene/systems/CameraSystem.h" "Prototypes/scene/systems/CameraSystem.cpp" "Prototypes/scene/components/TargetObjectComponent.h" "Prototypes/scene/components/TargetObjectComponent.cpp"   "Prototypes/miscellaneous/controllers/MiscellaneousController.h" "Prototypes/miscellaneous/controllers/MiscellaneousController.cpp")
	
if(EMSCRIPTEN)
	target_link_Libraries(CForgeSandbox
		tinyxml2
		crossforge
	)
	set_target_properties(CForgeSandbox PROPERTIES LINK_FLAGS "${LINK_FLAGS} ${Optimization_Flag} -sEXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1 -sWASM=1 -sUSE_WEBGL2=1 -fwasm-exceptions -sUSE_GLFW=3 -sUSE_ZLIB=1 -sUSE_LIBPNG=1 -sUSE_LIBJPEG=1 --preload-file Assets --preload-file Shader --preload-file MyAssets")

elseif(WIN32)
	target_include_directories(CForgeSandbox 
#	PRIVATE ${JPEG_INCLUDE_DIR}
	PRIVATE ${FFMPEG_INCLUDE_DIRS}
	)

	target_link_directories(CForgeSandbox
		PRIVATE ${FFMPEG_LIBRARY_DIRS}
	)

	target_link_libraries(CForgeSandbox 
	PRIVATE crossforge
	PRIVATE Eigen3::Eigen
	PRIVATE glfw 
	PRIVATE glad::glad
	ws2_32
	PRIVATE tinyfiledialogs::tinyfiledialogs
#	PRIVATE libjpeg-turbo::turbojpeg
#	PRIVATE WebP::webp
#	PRIVATE WebP::webpdecoder
#	PRIVATE tinyxml2::tinyxml2
#	PRIVATE imgui::imgui
	PRIVATE ${FFMPEG_LIBRARIES}
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
		PRIVATE Eigen3::Eigen
		PRIVATE glfw 
		PRIVATE glad::glad
		PRIVATE dl	
		)
endif()


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


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${crossforge_BINARY_DIR}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
