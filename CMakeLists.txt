cmake_minimum_required(VERSION 3.16)

SET(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics")    # Disable telemetry for vcpkg.
SET(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)             # Install vcpkg dependencies automatically(experimental - might be changed or removed later; see: https://github.com/microsoft/vcpkg/issues/1653). 

# Forbid in-source builds.
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	MESSAGE(SEND_ERROR "In-source builds are not allowed. Use a different build directory.")
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# CrossForge library
project(crossforge VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# create required directories
file(MAKE_DIRECTORY "MyAssets")
file(MAKE_DIRECTORY "Prototypes/Shader")

option(USE_OPENCV "Include OpenCV in build" OFF)


# required core packages
FIND_PACKAGE(OpenGL REQUIRED)		# OpenGl core library
FIND_PACKAGE(glad CONFIG REQUIRED)	# GL extension library
FIND_PACKAGE(glfw3 CONFIG REQUIRED)	# cross-plattform window management
FIND_PACKAGE(assimp CONFIG REQUIRED)# Asset import library (and partially export)
FIND_PACKAGE(Freetype REQUIRED)		# Library to load and process vector based fonts
FIND_PACKAGE(tinyxml2 CONFIG REQUIRED)	# support for xml loading, storing, and processing
FIND_PACKAGE(libigl CONFIG REQUIRED)	# mesh processing library
FIND_PACKAGE(WebP CONFIG REQUIRED)

if(USE_OPENCV)
	FIND_PACKAGE(OpenCV CONFIG REQUIRED)	# Open computer vision library
	include_directories(
		"${OpenCV_INCLUDE_DIRS}"
	)
	add_compile_definitions(USE_OPENCV)
endif(USE_OPENCV)

include_directories(
	"${FREETYPE_INCLUDE_DIRS}"
)

#enable edit and continue and hot reload (Visual Studio 2022)
#if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
#	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
#	string(REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS})
#	set(CMAKE_SHARED_LINKER_FLAGS "/SAFESEH:NO")
#	set(CMAKE_EXE_LINKER_FLAGS "/SAFESEH:NO")
#endif()

add_library(crossforge SHARED
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
	CForge/AssetIO/StbImageIO.cpp 
	CForge/AssetIO/WebPImageIO.cpp
	CForge/AssetIO/SAssetIO.cpp

	# Graphics related
	CForge/Graphics/GBuffer.cpp 
	CForge/Graphics/GLBuffer.cpp 
	CForge/Graphics/GLCubemap.cpp
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
	CForge/Graphics/Actors/SkyboxActor.cpp
	CForge/Graphics/Actors/VertexUtility.cpp 
	CForge/Graphics/Actors/ScreenQuad.cpp 
	CForge/Graphics/Actors/StaticActor.cpp 
	CForge/Graphics/Actors/SkeletalActor.cpp 
	CForge/Graphics/Actors/MorphTargetActor.cpp 

	# Animation Controller 
	CForge/Graphics/Controller/SkeletalAnimationController.cpp 
	CForge/Graphics/Controller/MorphTargetAnimationController.cpp

	# Shader
	CForge/Graphics/Shader/GLShader.cpp 
	CForge/Graphics/Shader/ShaderCode.cpp
	CForge/Graphics/Shader/SShaderManager.cpp

	# Uniform Buffer Objects
	CForge/Graphics/UniformBufferObjects/UBOCameraData.cpp 
	CForge/Graphics/UniformBufferObjects/UBOColorAdjustment.cpp
	CForge/Graphics/UniformBufferObjects/UBOLightData.cpp 
	CForge/Graphics/UniformBufferObjects/UBOMaterialData.cpp 
	CForge/Graphics/UniformBufferObjects/UBOModelData.cpp
	CForge/Graphics/UniformBufferObjects/UBOBoneData.cpp 
	CForge/Graphics/UniformBufferObjects/UBOMorphTargetData.cpp

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

	# Internet related
	CForge/Internet/UDPSocket.cpp
	CForge/Internet/TCPSocket.cpp

	# Mesh Processing
	CForge/MeshProcessing/Builder/MorphTargetModelBuilder.cpp

	# GUI
	CForge/GUI/Font.cpp
	CForge/GUI/GUI.cpp
	CForge/GUI/Widget.cpp
	CForge/GUI/WidgetBackground.cpp
	CForge/GUI/Widgets/Form.cpp
	CForge/GUI/Widgets/InputCheckbox.cpp
	CForge/GUI/Widgets/InputDropDown.cpp
	CForge/GUI/Widgets/InputNumber.cpp
	CForge/GUI/Widgets/InputSlider.cpp
	CForge/GUI/Widgets/InputText.cpp
	CForge/GUI/Widgets/Label.cpp
	CForge/GUI/Widgets/Window.cpp

 )

if(WIN32)
	add_compile_definitions(CFORGE_EXPORTS)
endif(WIN32)

if(UNIX)
	add_compile_definitions(USE_SYSFS_GPIO)
endif(UNIX)

if(WIN32)
target_link_libraries(crossforge 
	PRIVATE glfw 
	PRIVATE glad::glad
	PRIVATE assimp::assimp
	PRIVATE igl::core 
	PRIVATE igl::common		
	PRIVATE WebP::webp 
	PRIVATE WebP::webpdecoder
	ws2_32					#winsock2
	${FREETYPE_LIBRARIES}	# for Text rendering
	)
	if(USE_OPENCV)
	target_link_libraries(${OpenCV_LIBS})
	endif(USE_OPENCV)
endif(WIN32)

if(UNIX)
	target_link_libraries(crossforge 
	PRIVATE gpiod 
	PRIVATE stdc++fs
	PRIVATE glfw 
	PRIVATE assimp::assimp
	PRIVATE igl::core 
	PRIVATE igl::common
	PRIVATE glad::glad
)
	if(USE_OPENCV)
	target_link_libraries(${OpenCV_LIBS})
	endif(USE_OPENCV)
endif(UNIX)

project (CForgeSandbox)
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
#	${OpenCV_LIBS}
	PRIVATE glad::glad
	ws2_32
	PRIVATE tinyxml2::tinyxml2
	${FREETYPE_LIBRARIES}

	)
endif (WIN32)

if(UNIX)
	target_link_libraries(CForgeSandbox 
		PRIVATE crossforge
		PRIVATE glfw 
		PRIVATE assimp
		#PRIVATE igl::core 
		#PRIVATE igl::common
#		${OpenCV_LIBS}
		PRIVATE gpiod
		PRIVATE dl
		PRIVATE glad::glad
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
	${CMAKE_SOURCE_DIR}/Assets
	${CMAKE_CURRENT_BINARY_DIR}/Assets
)

add_custom_command(
	TARGET CForgeSandbox PRE_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
	${CMAKE_SOURCE_DIR}/MyAssets
	${CMAKE_CURRENT_BINARY_DIR}/MyAssets
)

#add_custom_command(
#	TARGET CForgeSandbox PRE_BUILD
#	COMMAND ${CMAKE_COMMAND} -E rm -r ${CMAKE_CURRENT_BINARY_DIR}/museumAssets
#)

#add_custom_command(
#	TARGET CForgeSandbox PRE_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory 
#	${CMAKE_SOURCE_DIR}/museumAssets
#	${CMAKE_CURRENT_BINARY_DIR}/museumAssets
#)

add_custom_command(
	TARGET CForgeSandbox PRE_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
	${CMAKE_SOURCE_DIR}/Prototypes/Shader 
	${CMAKE_CURRENT_BINARY_DIR}/Shader
)

project(ExportLibrary)
add_executable(ExportLibrary ExportLibrary.cpp)

target_link_libraries(ExportLibrary PRIVATE crossforge)

include_directories(
	"./"
)

project (Terrain)
add_executable(Terrain
	Main.cpp

#	Terrain/GUI/GUI.cpp
#	Terrain/GUI/WidgetBackground.cpp
#	Terrain/GUI/Widget.cpp
#	Terrain/GUI/Font.cpp
#	Terrain/GUI/Widgets/InputNumber.cpp
#	Terrain/GUI/Widgets/InputCheckbox.cpp
#	Terrain/GUI/Widgets/InputText.cpp
#	Terrain/GUI/Widgets/InputSlider.cpp
#	Terrain/GUI/Widgets/InputDropDown.cpp
#	Terrain/GUI/Widgets/Label.cpp
#	Terrain/GUI/Widgets/Form.cpp
#	Terrain/GUI/Widgets/Window.cpp

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

	#"${CMAKE_SOURCE_DIR}/thirdparty/glad/src/glad.c"
	#"${CMAKE_SOURCE_DIR}/thirdparty/tinyxml2/tinyxml2.cpp"
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
