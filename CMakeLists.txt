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
		"${CMAKE_SOURCE_DIR}/thirdparty/eigen"
		"${CMAKE_SOURCE_DIR}" # include top level for easy access to CForge, probably suboptimal
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
			PRIVATE assimp
			${OpenCV_LIBS}
			)
endif(UNIX)


project(ExportLibrary)
add_executable(ExportLibrary
		ExportLibrary.cpp
		)

target_link_libraries(ExportLibrary
		PRIVATE crossforge
		)

project (Terrain)
add_executable(Terrain
		Main.cpp
        Terrain/TileActor.cpp
		Terrain/Tile.cpp
		"${CMAKE_SOURCE_DIR}/thirdparty/glad/src/glad.c"
		)


if(WIN32)
	target_link_libraries(Terrain
			PRIVATE crossforge
			)
endif (WIN32)

if(UNIX)
	target_link_libraries(Terrain
			PRIVATE crossforge
			)
endif(UNIX)

# copy engine shaders
add_custom_command(
		TARGET Terrain PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/CForge/Shader
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