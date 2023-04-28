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
