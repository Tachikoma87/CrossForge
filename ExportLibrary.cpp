#include "CForge/AssetIO/File.h"
#ifdef WIN32
#include <filesystem>
using namespace std;
#else
#include <experimental/filesystem>
using namespace std::experimental;
#endif
#include <vector>

const std::string RootSrcDir = "../../../CForge/";
const std::string RootBinDir = "../";
const std::string RootExportDir = "../../";

#ifdef WIN32
void copyFile(filesystem::path From, filesystem::path To) {
	std::filesystem::path ToDir = To;
	ToDir.remove_filename();

	if (!filesystem::exists(From)) {
		printf("Error copying file %ls. It does not exist!\n", From.c_str());
	}
	else if (!std::filesystem::exists(ToDir)) {
		printf("Error copying file %ls. Directory %ls does not exist!\n", To.c_str(), ToDir.c_str());
	}
	else {
		filesystem::copy_file(From, To, std::filesystem::copy_options::overwrite_existing);
	}
}//copyFile
#else
void copyFile(filesystem::path From, filesystem::path To) {
	filesystem::path ToDir = To;
	ToDir.remove_filename();

#ifdef WIN32
	if (!filesystem::exists(From)) {
		printf("Error copying file %ls. It does not exist!\n", From.c_str());
	}
	else if (!filesystem::exists(ToDir)) {
		printf("Error copying file %ls. Directory %ls does not exist!\n", To.c_str(), ToDir.c_str());
	}
	else {
		filesystem::copy_file(From, To, filesystem::copy_options::overwrite_existing);
	}
#else
	if (!filesystem::exists(From)) {
		printf("Error copying file %s. It does not exist!\n", From.c_str());
	}
	else if (!filesystem::exists(ToDir)) {
		printf("Error copying file %s. Directory %s does not exist!\n", To.c_str(), ToDir.c_str());
	}
	else {
		filesystem::copy_file(From, To, filesystem::copy_options::overwrite_existing);
	}
#endif

}//copyFile

#endif

int main(int argc, char* argv[]) {

	std::vector<std::string> Directories;
	std::vector<std::string> IncludeFiles;
	std::vector<std::string> BinaryFiles;
	std::vector<std::string> LibFiles;

	Directories.push_back("crossforge/");
	Directories.push_back("crossforge/include/");
	Directories.push_back("crossforge/include/crossforge/");

	// Core include files
	Directories.push_back("crossforge/include/crossforge/Core");
	IncludeFiles.push_back("Core/CForgeObject.h");
	IncludeFiles.push_back("Core/CoreDefinitions.h");
	IncludeFiles.push_back("Core/CoreUtility.hpp");
	IncludeFiles.push_back("Core/CrossForgeException.h");
	IncludeFiles.push_back("Core/ITCaller.hpp");
	IncludeFiles.push_back("Core/ITListener.hpp");
	IncludeFiles.push_back("Core/SCrossForgeDevice.h");
	IncludeFiles.push_back("Core/SGPIO.h");
	IncludeFiles.push_back("Core/SLogger.h");

	// AssetIO include files.
	Directories.push_back("crossforge/include/crossforge/AssetIO/");
	IncludeFiles.push_back("AssetIO/File.h");
	IncludeFiles.push_back("AssetIO/I2DImageIO.h");
	IncludeFiles.push_back("AssetIO/I3DMeshIO.h");
	IncludeFiles.push_back("AssetIO/SAssetIO.h");
	IncludeFiles.push_back("AssetIO/T2DImage.hpp");
	IncludeFiles.push_back("AssetIO/T3DMesh.hpp");

	// Graphics
	Directories.push_back("crossforge/include/crossforge/Graphics/");
	IncludeFiles.push_back("Graphics/GBuffer.h");
	IncludeFiles.push_back("Graphics/GLBuffer.h");
	IncludeFiles.push_back("Graphics/GLTexture2D.h");
	IncludeFiles.push_back("Graphics/GLVertexArray.h");
	IncludeFiles.push_back("Graphics/GLWindow.h");
	IncludeFiles.push_back("Graphics/GraphicsUtility.h");
	IncludeFiles.push_back("Graphics/RenderDevice.h");
	IncludeFiles.push_back("Graphics/RenderMaterial.h");
	IncludeFiles.push_back("Graphics/STextureManager.h");
	IncludeFiles.push_back("Graphics/VirtualCamera.h");

	// Graphics/Actors
	Directories.push_back("crossforge/include/crossforge/Graphics/Actors/");
	IncludeFiles.push_back("Graphics/Actors/IRenderableActor.h");
	IncludeFiles.push_back("Graphics/Actors/MorphTargetActor.h");
	IncludeFiles.push_back("Graphics/Actors/RenderGroupUtility.h");
	IncludeFiles.push_back("Graphics/Actors/ScreenQuad.h");
	IncludeFiles.push_back("Graphics/Actors/SkeletalActor.h");
	IncludeFiles.push_back("Graphics/Actors/StaticActor.h");
	IncludeFiles.push_back("Graphics/Actors/VertexUtility.h");

	// Graphics/Controller
	Directories.push_back("crossforge/include/crossforge/Graphics/Controller/");
	IncludeFiles.push_back("Graphics/Controller/MorphTargetAnimationController.h");
	IncludeFiles.push_back("Graphics/Controller/SkeletalAnimationController.h");

	// Graphics/Lights
	Directories.push_back("crossforge/include/crossforge/Graphics/Lights/");
	IncludeFiles.push_back("Graphics/Lights/DirectionalLight.h");
	IncludeFiles.push_back("Graphics/Lights/ILight.h");
	IncludeFiles.push_back("Graphics/Lights/PointLight.h");
	IncludeFiles.push_back("Graphics/Lights/SpotLight.h");

	// Graphics/ScreenGraph
	Directories.push_back("crossforge/include/crossforge/Graphics/SceneGraph/");
	IncludeFiles.push_back("Graphics/SceneGraph/ISceneGraphNode.h");
	IncludeFiles.push_back("Graphics/SceneGraph/SceneGraph.h");
	IncludeFiles.push_back("Graphics/SceneGraph/SGNGeometry.h");
	IncludeFiles.push_back("Graphics/SceneGraph/SGNTransformation.h");

	// Graphics/Shader
	Directories.push_back("crossforge/include/crossforge/Graphics/Shader/");
	IncludeFiles.push_back("Graphics/Shader/GLShader.h");
	IncludeFiles.push_back("Graphics/Shader/SShaderManager.h");
	IncludeFiles.push_back("Graphics/Shader/ShaderCode.h");

	// Graphics/UniformBufferObjects
	Directories.push_back("crossforge/include/crossforge/Graphics/UniformBufferObjects/");
	IncludeFiles.push_back("Graphics/UniforMBufferObjects/UBOBoneData.h");
	IncludeFiles.push_back("Graphics/UniformBufferObjects/UBOCameraData.h");
	IncludeFiles.push_back("Graphics/UniformBUfferObjects/UBOLightData.h");
	IncludeFiles.push_back("Graphics/UniformBUfferObjects/UBOMaterialData.h");
	IncludeFiles.push_back("Graphics/UniformBufferObjects/UBOModelData.h");
	IncludeFiles.push_back("Graphics/UniformBufferObjects/UBOMorphTargetData.h");

	// INput
	Directories.push_back("crossforge/include/crossforge/Input/");
	IncludeFiles.push_back("Input/Keyboard.h");
	IncludeFiles.push_back("Input/Mouse.h");
	IncludeFiles.push_back("Input/SInputManager.h");


	// binary files
	Directories.push_back("crossforge/bin/");
	Directories.push_back("crossforge/lib/");

	Directories.push_back("crossforge/bin/x64-Debug/");
	Directories.push_back("crossforge/lib/x64-Debug/");
	LibFiles.push_back("x64-Debug/crossforge.lib");
	BinaryFiles.push_back("x64-Debug/assimp-vc142-mtd.dll");
	BinaryFiles.push_back("x64-Debug/bz2d.dll");
	BinaryFiles.push_back("x64-Debug/crossforge.dll");
	BinaryFiles.push_back("x64-Debug/glfw3.dll");
	BinaryFiles.push_back("x64-Debug/Irrlicht.dll");
	BinaryFiles.push_back("x64-Debug/jpeg62.dll");
	BinaryFiles.push_back("x64-Debug/libpng16d.dll");
	BinaryFiles.push_back("x64-Debug/lzmad.dll");
	BinaryFiles.push_back("x64-Debug/opencv_cored.dll");
//	BinaryFiles.push_back("x64-Debug/opencv_highguid.dll");
	BinaryFiles.push_back("x64-Debug/opencv_imgcodecsd.dll");
	BinaryFiles.push_back("x64-Debug/opencv_imgprocd.dll");
	BinaryFiles.push_back("x64-Debug/opencv_videoiod.dll");
	BinaryFiles.push_back("x64-Debug/tiffd.dll");
	BinaryFiles.push_back("x64-Debug/webpd.dll");
	BinaryFiles.push_back("x64-Debug/zlibd1.dll");

	Directories.push_back("crossforge/bin/x64-Release/");
	Directories.push_back("crossforge/lib/x64-Release/");
	LibFiles.push_back("x64-Release/crossforge.lib");
	BinaryFiles.push_back("x64-Release/assimp-vc142-mt.dll");
	BinaryFiles.push_back("x64-Release/bz2.dll");
	BinaryFiles.push_back("x64-Release/crossforge.dll");
	BinaryFiles.push_back("x64-Release/glfw3.dll");
	BinaryFiles.push_back("x64-Release/Irrlicht.dll");
	BinaryFiles.push_back("x64-Release/jpeg62.dll");
	BinaryFiles.push_back("x64-Release/libpng16.dll");
	BinaryFiles.push_back("x64-Release/lzma.dll");
	BinaryFiles.push_back("x64-Release/opencv_core.dll");
//	BinaryFiles.push_back("x64-Release/opencv_highgui.dll");
	BinaryFiles.push_back("x64-Release/opencv_imgcodecs.dll");
	BinaryFiles.push_back("x64-Release/opencv_imgproc.dll");
	BinaryFiles.push_back("x64-Release/opencv_videoio.dll");
	BinaryFiles.push_back("x64-Release/tiff.dll");
	BinaryFiles.push_back("x64-Release/webp.dll");
	BinaryFiles.push_back("x64-Release/zlib1.dll");

	// create all directories
	for (auto i : Directories) filesystem::create_directories(RootExportDir + i);

	// copy include files
	for (auto i : IncludeFiles) {
		filesystem::path From = RootSrcDir + i;
		filesystem::path To = RootExportDir + "crossforge/include/crossforge/" + i;
		copyFile(From, To);
	}//for(include files)

	// copy lib files
	for (auto i : LibFiles) {
		filesystem::path From = RootBinDir + i;
		filesystem::path To = RootExportDir + "crossforge/lib/" + i;
		copyFile(From, To);
	}//for[library files]

	for (auto i : BinaryFiles) {
		filesystem::path From = RootBinDir + i;
		filesystem::path To = RootExportDir + "crossforge/bin/" + i;
		copyFile(From, To);
	}


	char c;
	printf("Copy operations completed!\n");
	scanf("%c", &c);

}//main
