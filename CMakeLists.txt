cmake_minimum_required(VERSION 3.16)

# Forbid in-source builds.
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   MESSAGE(SEND_ERROR "In-source builds are not allowed. Use a different build directory.")
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# CrossForge library
project(crossforge VERSION 0.1 LANGUAGES CXX)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(assimp CONFIG REQUIRED)
FIND_PACKAGE(glfw3 CONFIG REQUIRED)
FIND_PACKAGE(OpenCV CONFIG REQUIRED)



include_directories(
	"${CMAKE_SOURCE_DIR}/thirdparty/glad/include"
	"${CMAKE_SOURCE_DIR}/thirdparty/eigen-3.4.0"
	"${OpenCV_INCLUDE_DIRS}"
)

add_library(crossforge SHARED 
	# GLAD
	"${CMAKE_SOURCE_DIR}/thirdparty/glad/src/glad.c"

	# Core related
	CForge/Core/CForgeObject.cpp 
	CForge/Core/CrossForgeException.cpp
	CForge/Core/SCrossForgeDevice.cpp
	CForge/Core/SGPIO.cpp
	CForge/Core/SLogger.cpp
	CForge/Core/CoreUtility.cpp

	# Asset import/exporter stuff
	CForge/AssetIO/File.cpp
	CForge/AssetIO/AssimpMeshIO.cpp
	CForge/AssetIO/I2DImageIO.cpp
	CForge/AssetIO/I3DMeshIO.cpp
	CForge/AssetIO/OpenCVImageIO.cpp
	CForge/AssetIO/SAssetIO.cpp

	# Graphics related
	CForge/Graphics/GBuffer.cpp 
	CForge/Graphics/GLBuffer.cpp 
	CForge/Graphics/GLTexture2D.cpp 
	CForge/Graphics/GLVertexArray.cpp 
	CForge/Graphics/GLWindow.cpp 
	CForge/Graphics/RenderDevice.cpp 
	CForge/Graphics/RenderMaterial.cpp 
	CForge/Graphics/STextureManager.cpp 
	CForge/Graphics/VirtualCamera.cpp
	CForge/Graphics/GraphicsUtility.cpp

	# Actor related
	CForge/Graphics/Actors/IRenderableActor.cpp 
	CForge/Graphics/Actors/RenderGroupUtility.cpp 
	CForge/Graphics/Actors/ScreenQuad.cpp 
	CForge/Graphics/Actors/StaticActor.cpp 
	CForge/Graphics/Actors/VertexUtility.cpp 

	# Shader
	CForge/Graphics/Shader/GLShader.cpp 
	CForge/Graphics/Shader/ShaderCode.cpp
	CForge/Graphics/Shader/SShaderManager.cpp

	# Uniform Buffer Objects
	CForge/Graphics/UniformBufferObjects/UBOCameraData.cpp 
	CForge/Graphics/UniformBufferObjects/UBOLightData.cpp 
	CForge/Graphics/UniformBufferObjects/UBOMaterialData.cpp 
	CForge/Graphics/UniformBufferObjects/UBOModelData.cpp

	# Lights
	CForge/Graphics/Lights/ILight.cpp 
	CForge/Graphics/Lights/DirectionalLight.cpp 
	CForge/Graphics/Lights/PointLight.cpp 
	CForge/Graphics/Lights/SpotLight.cpp

	# SceneGraph
	CForge/Graphics/SceneGraph/ISceneGraphNode.cpp
	CForge/Graphics/SceneGraph/SceneGraph.cpp 
	CForge/Graphics/SceneGraph/SGNGeometry.cpp
	CForge/Graphics/SceneGraph/SGNTransformation.cpp

	# Input related 
	CForge/Input/Keyboard.cpp 
	CForge/Input/Mouse.cpp
	CForge/Input/SInputManager.cpp
 )
set_target_properties(crossforge PROPERTIES VERSION ${PROJECT_VERSION})

if(WIN32)
add_compile_definitions(CFORGE_EXPORTS)
endif(WIN32)

if(UNIX)
add_compile_definitions(USE_SYSFS_GPIO)
endif(UNIX)

if(WIN32)
target_link_libraries(crossforge 
	PRIVATE glfw 
	PRIVATE assimp::assimp
	${OpenCV_LIBS}
	)
endif(WIN32)

if(UNIX)
	target_link_libraries(crossforge 
	PRIVATE gpiod 
	PRIVATE stdc++fs
	PRIVATE glfw 
	PRIVATE assimp::assimp
	${OpenCV_LIBS}
)
endif(UNIX)

project (CForgeSandbox)
add_executable(CForgeSandbox 
	SandboxMain.cpp 
	"thirdparty/glad/src/glad.c"

	CForge/Core/SGPIO.cpp
	Prototypes/I2C.cpp 
	Prototypes/MPU6050.cpp
	Prototypes/RotaryEncoder.cpp
	Prototypes/PhotoFinish.cpp
	Prototypes/StripPhoto.cpp 
	Prototypes/StripPhotoCamera.cpp

	Prototypes/Actor/SkeletalActor.cpp
	Prototypes/Actor/UBOBoneData.cpp
	Prototypes/Actor/SkeletalAnimationController.cpp
	Prototypes/Actor/MorphTargetActor.cpp 
	Prototypes/Actor/MorphTargetAnimationController.cpp 
	Prototypes/Actor/UBOMorphTargetData.cpp 

	Prototypes/Builder/MorphTargetBuilder.cpp
	)


if(WIN32)
	target_link_libraries(CForgeSandbox 
	PRIVATE crossforge
	PRIVATE glfw 
	${OpenCV_LIBS}
	)
endif (WIN32)

if(UNIX)
	target_link_libraries(CForgeSandbox 
		PRIVATE crossforge
		PRIVATE glfw 
		PRIVATE assimp::assimp
		${OpenCV_LIBS}
		PRIVATE gpiod
		PRIVATE dl
		)
endif(UNIX)

#copy shaders
add_custom_command(
	TARGET CForgeSandbox PRE_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/Prototypes/Shader
	${CMAKE_CURRENT_BINARY_DIR}/Shader
)


project(ExportLibrary)
add_executable(ExportLibrary 
	ExportLibrary.cpp)

target_link_libraries(ExportLibrary
	PRIVATE crossforge)
