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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# create required directories if non existent 
file(MAKE_DIRECTORY "MyAssets")
file(MAKE_DIRECTORY "Prototypes")
file(MAKE_DIRECTORY "Prototypes/shader")

#################################################################
########### Project - CrossForge Sandbox ########################
#################################################################
project (CForgeSandbox)

option(CROSSFORGE_OPTION_FFMPEG "Features related to videos" OFF)

include(FetchContent)


FetchContent_Declare(
	libcrossforge
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/libcrossforge"
)
FetchContent_MakeAvailable(libcrossforge)

FetchContent_Declare(
	stb 
	GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)
include_directories(${stb_SOURCE_DIR})


if(EMSCRIPTEN)
	### Eigen
	FetchContent_Declare(
		eigen 
		GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
	)
	FetchContent_MakeAvailable(eigen)
	include_directories(${eigen_SOURCE_DIR}/)

	## WebP Library
	FetchContent_Declare(
		webp 
		GIT_REPOSITORY https://github.com/webmproject/libwebp.git
		GIT_TAG v1.6.0
	)
	FetchContent_MakeAvailable(webp)
	FetchContent_GetProperties(webp)
	include_directories(${webp_SOURCE_DIR}/src/)
	link_directories(${webp_BINARY_DIR})

	## Assimp
	FetchContent_Declare(
		assimp 
		URL https://github.com/assimp/assimp/archive/refs/tags/v6.0.3.zip
	)
	FetchContent_MakeAvailable(assimp)
	include_directories(${assimp_SOURCE_DIR}/include/)
	include_directories(${assimp_BINARY_DIR}/include/)
	link_directories(${assimp_BINARY_DIR}/lib/)

	## libigl
	FetchContent_Declare(
		libigl 
		URL https://github.com/libigl/libigl/archive/refs/tags/v2.6.0.zip
	)
	FetchContent_MakeAvailable(libigl)
	include_directories(${libigl_SOURCE_DIR}/include/)

	## freetype
	FetchContent_Declare(
		freetype 
		GIT_REPOSITORY https://github.com/freetype/freetype.git
		#GIT_TAG v2.0.3
	)
	FetchContent_MakeAvailable(freetype)
	include_directories(${freetype_SOURCE_DIR}/include/)

	# tinygltf
	FetchContent_Declare(
		tinygltf 
		URL https://github.com/syoyo/tinygltf/archive/refs/tags/v2.9.7.zip
	)
	FetchContent_MakeAvailable(tinygltf)
	include_directories(${tinygltf_SOURCE_DIR}/)

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${Optimization_Flag} -fwasm-exceptions -Wno-deprecated -Wno-unused-command-line-argument -sUSE_ZLIB=1 -sUSE_GLFW=3 -sUSE_LIBPNG=1 -sUSE_LIBJPEG=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Optimization_Flag} -std=c++20 -fwasm-exceptions -Wno-deprecated -Wno-unused-command-line-argument -Wno-tautological-pointer-compare -sUSE_ZLIB=1 -sUSE_GLFW=3 -sUSE_LIBPNG=1 -sUSE_LIBJPEG=1")


	set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
# Additional packages not alreday included in the core library
	FIND_PACKAGE(tinyxml2 CONFIG REQUIRED)	# support for xml loading, storing, and processing
	FIND_PACKAGE(imgui CONFIG REQUIRED) # dear-imgui
	FIND_PACKAGE(glad CONFIG REQUIRED)	# GL extension library
	FIND_PACKAGE(glfw3 CONFIG REQUIRED)	# cross-plattform window management
	FIND_PACKAGE(Eigen3 CONFIG REQUIRED)
	find_package(libpqxx CONFIG REQUIRED)
	find_package(Drogon CONFIG REQUIRED)


#	FIND_PACKAGE(freetype REQUIRED)		# Library to load and process vector based fonts
#	FIND_PACKAGE(libigl CONFIG REQUIRED)# mesh processing library

endif()



add_executable(CForgeSandbox 

	################################
	#### New CrossForge ############
	################################
	"CrossForgeSandbox.cpp" 
	"Prototypes/applications/SCrossForgeTestApp.cpp"

#	"Prototypes/database/router/TestRouter.cpp"
#	"Prototypes/database/daos/DatabaseVersionDao.h"
#	"Prototypes/database/daos/DatabaseVersionDao.cpp"
#	"Prototypes/database/pocos/DatabaseVersionPoco.h"
#	"Prototypes/database/pocos/DatabaseVersionPoco.cpp"

  
#	"Prototypes/applications/SCrossForgeRestTestApp.cpp" 
    "Prototypes/applications/SCrossForgeSimpleSceneApp.h" 
	 "Prototypes/applications/SCrossForgeSimpleSceneApp.cpp" 

	 "Prototypes/scene/controllers/LightEntityController.h"
	 "Prototypes/scene/controllers/LightEntityController.cpp"
	 
	 
	 "Prototypes/scene/entities/LightEntity.cpp"

	 "Prototypes/scene/controllers/ActorInstanceEntityController.h"
	 "Prototypes/scene/controllers/ActorInstanceEntityController.cpp"
   
   "Prototypes/examples/ExampleBase.h" 
   "Prototypes/examples/ExampleBase.cpp" 
   "Prototypes/examples/BasicWindowSetup.h" 
   "Prototypes/examples/BasicWindowSetup.cpp"
)

	
if(EMSCRIPTEN)
	target_link_Libraries(CForgeSandbox
		crossforge
	)
	set_target_properties(CForgeSandbox PROPERTIES LINK_FLAGS "${LINK_FLAGS} ${Optimization_Flag} -sEXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1 -sWASM=1 -sUSE_WEBGL2=1 -fwasm-exceptions -sUSE_GLFW=3 -sUSE_ZLIB=1 -sUSE_LIBPNG=1 -sUSE_LIBJPEG=1 --preload-file Assets")

elseif(WIN32)
	target_link_libraries(CForgeSandbox 
	PRIVATE crossforge
	PRIVATE Eigen3::Eigen
	PRIVATE glfw 
	PRIVATE glad::glad
#	PRIVATE oatpp
	PRIVATE libpqxx::pqxx
	PRIVATE Drogon::Drogon
	ws2_32
	PRIVATE libpqxx::pqxx

#	PRIVATE tinyxml2::tinyxml2
#	PRIVATE imgui::imgui
#	${FREETYPE_LIBRARIES}
#	PRIVATE igl::igl_core 	
#	PRIVATE Freetype::Freetype
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
