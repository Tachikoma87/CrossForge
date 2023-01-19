/*****************************************************************************\
*                                                                           *
* File(s): ForwardTestScene.hpp                                            *
*                                                                           *
* Content:    *
*                        *
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
#ifndef __CFORGE_FORWARDTESTSCENE_HPP__
#define __CFORGE_FORWARDTESTSCENE_HPP__


#include "../../Examples/ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ForwardTestScene : public ExampleSceneBase {
	public:
		ForwardTestScene(void) {
			m_WindowTitle = "CrossForge Test Scene - Forward Rendering Test";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ForwardTestScene(void) {
			clear();
		}//Destructor

		void init() {

			initWindowAndRenderDevice();
			initCameraAndLights();

			// load skydome and a textured cube
			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/StarCoin/StarCoin.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			//M.computePerVertexTangents();
			m_Cube.init(&M);
			M.clear();




			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add cube
			m_CubeTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
			m_CubeSGN.init(&m_CubeTransformSGN, &m_Cube);
			m_CubeSGN.scale(Vector3f(3.0f, 3.0f, 3.0f));

			// rotate about the y-axis at 45 degree every second
			Quaternionf R;
			R = AngleAxisf(GraphicsUtility::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			m_CubeTransformSGN.rotationDelta(R);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CoreUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			GraphicsUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear

		bool Deferred = false;

		void run(void) {
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_SG.render(&m_RenderDev);

				if (Deferred) {
					m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
					m_SG.render(&m_RenderDev);

					m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
				}
				else {
					m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, true);
					m_SG.render(&m_RenderDev);
				}


				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_5, true)) {
					T2DImage<uint8_t> DepthImg;
					m_Sun.retrieveDepthBuffer(&DepthImg);
					SAssetIO::store("SunDepth.jpg", &DepthImg);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_6, true)) {
					Deferred = !Deferred;
				}
				

			}//while[main loop]
		}//run

	protected:

		void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) override {
			for (uint32_t i = 0; i < pM->materialCount(); ++i) {
				T3DMesh<float>::Material* pMat = pM->getMaterial(i);

				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");

				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");


				pMat->VertexShaderForwardPass.push_back("Shader/ForwardPassPBS.vert");
				pMat->FragmentShaderForwardPass.push_back("Shader/ForwardPassPBS.frag");

				pMat->Metallic = Metallic;
				pMat->Roughness = Roughness;
			}//for[materials]
		}//setMeshShader


		// Scene Graph
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_CubeSGN;
		SGNTransformation m_CubeTransformSGN;

		StaticActor m_Skydome;
		StaticActor m_Cube;

	};//ForwardTestScene

}//name space

#endif