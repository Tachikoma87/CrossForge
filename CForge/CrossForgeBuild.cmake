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

option(USE_OPENCV "Include OpenCV in build" OFF)

include(FetchContent)

#[[
## download and install pmp
FetchContent_Declare(
	pmp 
	GIT_REPOSITORY https://github.com/pmp-library/pmp-library.git
	GIT_TAG 2.0.1
)
FetchContent_MakeAvailable(pmp)
set(PMP_BUILD_APPS OFF CACHE INTERNAL "Build the PMP applications")
set(PMP_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the PMP examples")
set(PMP_BUILD_TESTS OFF CACHE INTERNAL "Build the PMP test programs")
set(PMP_BUILD_DOCS OFF CACHE INTERNAL "Build the PMP documentation")
set(PMP_BUILD_VIS OFF CACHE INTERNAL "Build the PMP visualization tools")
set(PMP_INSTALL OFF CACHE INTERNAL "Install the PMP library and headers")
]]


# required core packages
FIND_PACKAGE(Eigen3 CONFIG REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)		# OpenGl core library
FIND_PACKAGE(glad CONFIG REQUIRED)	# GL extension library
FIND_PACKAGE(glfw3 CONFIG REQUIRED)	# cross-plattform window management
FIND_PACKAGE(assimp CONFIG REQUIRED)# Asset import library (and partially export)
FIND_PACKAGE(freetype REQUIRED)		# Library to load and process vector based fonts
FIND_PACKAGE(libigl CONFIG REQUIRED)	# mesh processing library
FIND_PACKAGE(WebP CONFIG REQUIRED)	# WebP to import/export webp 

if(USE_OPENCV)
	FIND_PACKAGE(OpenCV CONFIG REQUIRED)	# Open computer vision library
	include_directories(
		"${OpenCV_INCLUDE_DIRS}"
	)
	add_compile_definitions(USE_OPENCV)
else()
	set(OpenCV_LIBS "")
	remove_definitions(USE_OPENCV)
endif(USE_OPENCV)

include_directories(
#	"${FREETYPE_INCLUDE_DIRS}"
	"Thirdparty/stb/"
	"./"
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

	# Camera related
	CForge/Graphics/Camera/ViewFrustum.cpp


	# Actor related
	CForge/Graphics/Actors/IRenderableActor.cpp 
	CForge/Graphics/Actors/RenderGroupUtility.cpp 
	CForge/Graphics/Actors/SkyboxActor.cpp
	CForge/Graphics/Actors/VertexUtility.cpp 
	CForge/Graphics/Actors/ScreenQuad.cpp 
	CForge/Graphics/Actors/StaticActor.cpp 
	CForge/Graphics/Actors/SkeletalActor.cpp 
	CForge/Graphics/Actors/MorphTargetActor.cpp 
	CForge/Graphics/Actors/StickFigureActor.cpp

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

	# Math
	CForge/Math/BoundingVolume.cpp

	# Input related
	CForge/Input/Keyboard.cpp
	CForge/Input/Mouse.cpp	
	CForge/Input/SInputManager.cpp

	# Internet related
	CForge/Internet/UDPSocket.cpp
	CForge/Internet/TCPSocket.cpp

	# Mesh Processing
	CForge/MeshProcessing/Builder/MorphTargetModelBuilder.cpp
	CForge/MeshProcessing/PrimitiveShapeFactory.cpp

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
	PRIVATE Eigen3::Eigen
	PRIVATE glfw 
	PRIVATE glad::glad
	PRIVATE assimp::assimp
	PRIVATE igl::core 
	PRIVATE igl::common		
	PRIVATE WebP::webp 
	PRIVATE WebP::webpdecoder
#	pmp
	ws2_32					#winsock2
	${FREETYPE_LIBRARIES}	# for Text rendering
	${OpenCV_LIBS}
	)

elseif(UNIX)
	target_link_libraries(crossforge 
	PRIVATE Eigen3::Eigen
	PRIVATE glfw
	PRIVATE glad::glad
	PRIVATE assimp::assimp
	PRIVATE igl::core 
	PRIVATE igl::common
	PRIVATE WebP::webp 
	PRIVATE WebP::webpdecoder
	${FREETYPE_LIBRARIES}	# for Text rendering
	freetype
#	${OpenCV_LIBS}

	PRIVATE gpiod 
	PRIVATE stdc++fs
	)
endif(WIN32)
