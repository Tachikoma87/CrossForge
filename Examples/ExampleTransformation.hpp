/*****************************************************************************\
*                                                                           *
* File(s): ExampleTransformation.hpp                                        *
*                                                                           *
* Content:	Example scene that shows how transformations are applied with	*
*			and without scenegraph.											*
*		                                                                    *
*                                                                           *
* Author(s): Max Meyer, Tom Uhlmann                                         *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_EXAMPLETRANSFORMATION_HPP__
#define __CFORGE_EXAMPLETRANSFORMATION_HPP__


#include "ExampleSceneBase.hpp"
#include "../CForge/Graphics/Actors/SkyboxActor.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleTransformation : public ExampleSceneBase {
	public:
		ExampleTransformation(void) {
			m_WindowTitle = "CrossForge Example - Transformation";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ExampleTransformation(void) {
			clear();
		}//Destructor

		void init() {

			initWindowAndRenderDevice();
			initCameraAndLights();

			// load skybox and planetsystem
			
			vector<string> m_EmptySpace;

			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_right.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_left.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_up.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_down.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_back.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_front.png");


			m_SpaceSkybox.init(m_EmptySpace[0], m_EmptySpace[1], m_EmptySpace[2], m_EmptySpace[3], m_EmptySpace[4], m_EmptySpace[5]);

			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/Solar/sun/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_SunBody.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Solar/mercury/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Merkur.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Solar/venus/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Venus.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Solar/earth/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Earth.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Solar/mars/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Mars.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Solar/jupiter/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Jupiter.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Solar/saturn/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Saturn.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Solar/uranus/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Uranus.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Solar/neptune/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Neptune.init(&M);
			M.clear();

			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);
			
			// add skydome
			m_SpaceSkybox.brightness(1.15f);
			m_SpaceSkybox.contrast(1.1f);
			m_SpaceSkybox.saturation(1.2f);

			m_SpaceSkyboxTransSGN.init(nullptr);
			m_SpaceSkyboxSGN.init(&m_SpaceSkyboxTransSGN, &m_SpaceSkybox);
			m_SpaceSkyboxSG.init(&m_SpaceSkyboxTransSGN);
			
			// add planets
			m_SolarsystemSGN.init(&m_RootSGN, Vector3f(0.0f, 2.0f, 0.0f));

			m_SunSGN.init(&m_SolarsystemSGN, &m_SunBody);
			m_SunSGN.scale(Vector3f(0.001f, 0.001f, 0.001f));


			// Merkur
			m_MerkurOrbitSGN.init(&m_SolarsystemSGN);
			m_MerkurTransformSGN.init(&m_MerkurOrbitSGN, Vector3f(1.5f, 0.0f, 0.0f));
			m_MerkurSGN.init(&m_MerkurTransformSGN, &m_Merkur);
			m_MerkurSGN.scale(Vector3f(0.003f, 0.003f, 0.003f));

			// orbiting
			Quaternionf R;
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_MerkurOrbitSGN.rotationDelta(R);

			// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_MerkurTransformSGN.rotationDelta(R);

			// Venus
			m_VenusOrbitSGN.init(&m_SolarsystemSGN);
			m_VenusTransformSGN.init(&m_VenusOrbitSGN, Vector3f(2.0f, 0.0f, 0.0f));
			m_VenusSGN.init(&m_VenusTransformSGN, &m_Venus);
			m_VenusSGN.scale(Vector3f(0.003f, 0.003f, 0.003f));

			// orbiting
			R = AngleAxisf(CForgeMath::degToRad(42.0f / 120.0f), Vector3f::UnitY());
			m_VenusOrbitSGN.rotationDelta(R);

			// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_VenusTransformSGN.rotationDelta(R);

			// Earth
			m_EarthOrbitSGN.init(&m_SolarsystemSGN);
			m_EarthTransformSGN.init(&m_EarthOrbitSGN, Vector3f(2.5f, 0.0f, 0.0f));
			m_EarthSGN.init(&m_EarthTransformSGN, &m_Earth);
			m_EarthSGN.scale(Vector3f(0.3f, 0.3f, 0.3f));

			// orbiting
			R = AngleAxisf(CForgeMath::degToRad(40.0f / 120.0f), Vector3f::UnitY());
			m_EarthOrbitSGN.rotationDelta(R);

			// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_EarthTransformSGN.rotationDelta(R);

			// Mars
			m_MarsOrbitSGN.init(&m_SolarsystemSGN);
			m_MarsTransformSGN.init(&m_MarsOrbitSGN, Vector3f(3.2f, 0.0f, 0.0f));
			m_MarsSGN.init(&m_MarsTransformSGN, &m_Mars);
			m_MarsSGN.scale(Vector3f(0.003f, 0.003f, 0.003f));

			// orbiting
			R = AngleAxisf(CForgeMath::degToRad(36.0f / 120.0f), Vector3f::UnitY());
			m_MarsOrbitSGN.rotationDelta(R);

			// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_MarsTransformSGN.rotationDelta(R);

			// Jupiter
			m_JupiterOrbitSGN.init(&m_SolarsystemSGN);
			m_JupiterTransformSGN.init(&m_JupiterOrbitSGN, Vector3f(4.2f, 0.0f, 0.0f));
			m_JupiterSGN.init(&m_JupiterTransformSGN, &m_Jupiter);
			m_JupiterSGN.scale(Vector3f(0.003f, 0.003f, 0.003f));

			// orbiting
			R = AngleAxisf(CForgeMath::degToRad(30.0f / 120.0f), Vector3f::UnitY());
			m_JupiterOrbitSGN.rotationDelta(R);

			// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_JupiterTransformSGN.rotationDelta(R);

			// Saturn
			m_SaturnOrbitSGN.init(&m_SolarsystemSGN);
			m_SaturnTransformSGN.init(&m_SaturnOrbitSGN, Vector3f(5.2f, 0.0f, 0.0f));
			m_SaturnSGN.init(&m_SaturnTransformSGN, &m_Saturn);
			m_SaturnSGN.scale(Vector3f(0.003f, 0.003f, 0.003f));

			// orbiting
			R = AngleAxisf(CForgeMath::degToRad(27.0f / 120.0f), Vector3f::UnitY());
			m_SaturnOrbitSGN.rotationDelta(R);

			// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_SaturnTransformSGN.rotationDelta(R);

			// Uranus
			m_UranusOrbitSGN.init(&m_SolarsystemSGN);
			m_UranusTransformSGN.init(&m_UranusOrbitSGN, Vector3f(6.4f, 0.0f, 0.0f));
			m_UranusSGN.init(&m_UranusTransformSGN, &m_Uranus);
			m_UranusSGN.scale(Vector3f(0.003f, 0.003f, 0.003f));

			// orbiting
			R = AngleAxisf(CForgeMath::degToRad(22.0f / 120.0f), Vector3f::UnitY());
			m_UranusOrbitSGN.rotationDelta(R);

			// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_UranusTransformSGN.rotationDelta(R);

			// Neptune
			m_NeptuneOrbitSGN.init(&m_SolarsystemSGN);
			m_NeptuneTransformSGN.init(&m_NeptuneOrbitSGN, Vector3f(7.8f, 0.0f, 0.0f));
			m_NeptuneSGN.init(&m_NeptuneTransformSGN, &m_Neptune);
			m_NeptuneSGN.scale(Vector3f(0.003f, 0.003f, 0.003f));

			// orbiting
			R = AngleAxisf(CForgeMath::degToRad(19.0f / 120.0f), Vector3f::UnitY());
			m_NeptuneOrbitSGN.rotationDelta(R);

			// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_NeptuneTransformSGN.rotationDelta(R);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear

		void mainloop(void) {
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				//m_SpaceSkyboxSG.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
				
				m_SpaceSkyboxSG.render(&m_RenderDev);

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

			}//while[main loop]
		}//run

	protected:

		// Scene Graph
		SceneGraph m_SpaceSkyboxSG;
		SGNTransformation m_SpaceSkyboxTransSGN;
		SGNTransformation m_RootSGN;
		SGNGeometry m_SpaceSkyboxSGN;

		SGNTransformation m_SolarsystemSGN;

		SGNGeometry m_SunSGN;
		SGNGeometry m_MerkurSGN;
		SGNTransformation m_MerkurOrbitSGN;
		SGNTransformation m_MerkurTransformSGN;
		SGNGeometry m_VenusSGN;
		SGNTransformation m_VenusOrbitSGN;
		SGNTransformation m_VenusTransformSGN;
		SGNGeometry m_EarthSGN;
		SGNTransformation m_EarthOrbitSGN;
		SGNTransformation m_EarthTransformSGN;
		SGNGeometry m_MarsSGN;
		SGNTransformation m_MarsOrbitSGN;
		SGNTransformation m_MarsTransformSGN;
		SGNGeometry m_JupiterSGN;
		SGNTransformation m_JupiterOrbitSGN;
		SGNTransformation m_JupiterTransformSGN;
		SGNGeometry m_SaturnSGN;
		SGNTransformation m_SaturnOrbitSGN;
		SGNTransformation m_SaturnTransformSGN;
		SGNGeometry m_UranusSGN;
		SGNTransformation m_UranusOrbitSGN;
		SGNTransformation m_UranusTransformSGN;
		SGNGeometry m_NeptuneSGN;
		SGNTransformation m_NeptuneOrbitSGN;
		SGNTransformation m_NeptuneTransformSGN;

		SkyboxActor m_SpaceSkybox;

		StaticActor m_SunBody;
		StaticActor m_Merkur;
		StaticActor m_Venus;
		StaticActor m_Earth;
		StaticActor m_Mars;
		StaticActor m_Jupiter;
		StaticActor m_Saturn;
		StaticActor m_Uranus;
		StaticActor m_Neptune;

	};//ExampleTransformation

}

#endif