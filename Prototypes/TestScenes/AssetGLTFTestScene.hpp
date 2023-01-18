/*****************************************************************************\
*                                                                           *
* File(s): exampleMinimumGraphicsSetup.hpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
*          window, lighting setup, and a single moving object.              *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_ASSETGLTFTESTSCENE_HPP__
#define __CFORGE_ASSETGLTFTESTSCENE_HPP__

#ifndef __unix__

#include "../../Examples/exampleSceneBase.hpp"
#include "../../Prototypes/Assets/GLTFIO.h"


#include <filesystem>
#include <iostream>

using namespace Eigen;
using namespace std;

namespace CForge {

	class AssetGLTFTestScene : public ExampleSceneBase {
	public:
		AssetGLTFTestScene(void) {
			m_WindowTitle = "CrossForge - GLTF Test Scene";
		}//Constructor

		~AssetGLTFTestScene(void) {
			clear();
		}//Destructor

		void init(void) {

			initWindowAndRenderDevice();
			m_RenderWin.title(m_WindowTitle);

			initCameraAndLights();

			// load skydome and a textured cube
			T3DMesh<float> M, testModel;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();


			//gltf testing
			GLTFIO gltfio;
			/*
			std::string name = "AnimatedCube";

			std::string gltfPath = "Assets/gltf sample models/" + name + "/glTF/" + name;

			gltfio.load(gltfPath + ".gltf", &testModel);
			gltfio.store(gltfPath + "_debug.gltf", &testModel);
			testModel.clear();
			gltfio.load(gltfPath + "_debug.gltf", &testModel);
			*/


			//std::string file = "C:/Users/Ossit/Downloads/Kuratchi_ver.1.0/Kuratchi_ver.1.0/high/fbx_rigged";
			std::string file = "Assets/Examplescenes/Helmet/simple_skin.gltf";
			std::string output = std::filesystem::path(file).replace_extension("gltf").string();

			auto absolute_path = std::filesystem::absolute(file);

			if (!std::filesystem::exists(file)) {
				std::cout << "File does not exist: " << absolute_path << std::endl;
				return;
			}

			SAssetIO::load(file, &testModel);
			assert(testModel.submeshCount() > 0);

			testModel.computePerVertexNormals();

			gltfio.store(output, &testModel);

			testModel.clear();

			gltfio.load(output, &testModel);


			setMeshShader(&testModel, 0.1f, 0.04f);
			testModel.computePerVertexNormals();
			testModel.bones(&std::vector<T3DMesh<float>::Bone*>(), false);
			m_Cube.init(&testModel);
			testModel.clear();


			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add cube
			m_CubeTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
			m_CubeSGN.init(&m_CubeTransformSGN, &m_Cube);

			// rotate about the y-axis at 45 degree every second
			Quaternionf R;
			R = AngleAxisf(GraphicsUtility::degToRad(45.0f / 20000.0f), Vector3f::UnitY());
			m_CubeTransformSGN.rotationDelta(R);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CoreUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			GraphicsUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());
		}//initialize

		void clear(void) {

		}//clear

		void release(void) {
			delete this;
		}//release

		void run(void) {
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f/m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());
			}//while[main loop]

		}//run

	protected:
		StaticActor m_Skydome;
		StaticActor m_Cube;

		SceneGraph m_SG;
		SGNTransformation m_RootSGN;

		SGNGeometry m_SkydomeSGN;

		SGNGeometry m_CubeSGN;
		SGNTransformation m_CubeTransformSGN;

	};//AssetGLTFTestScene

}

#endif
#endif