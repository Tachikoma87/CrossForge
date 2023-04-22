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
		}//Constructor

		~ExampleTransformation(void) {
			clear();
		}//Destructor

		void init() override {

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


			m_Skybox.init(m_EmptySpace[0], m_EmptySpace[1], m_EmptySpace[2], m_EmptySpace[3], m_EmptySpace[4], m_EmptySpace[5]);

			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/Solar/sun/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_SunBody.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Solar/mercury/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Mercury.init(&M);
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
			m_Skybox.brightness(1.15f);
			m_Skybox.contrast(1.1f);
			m_Skybox.saturation(1.2f);

			m_SkyboxTransSGN.init(nullptr);
			m_SkyboxSGN.init(&m_SkyboxTransSGN, &m_Skybox);
			m_SkyboxSG.init(&m_SkyboxTransSGN);
			
			// Solarsystem
			m_SolarsystemSGN.init(&m_RootSGN, Vector3f(0.0f, 0.0f, 0.0f));

			//Sun
			m_SunSGN.init(&m_SolarsystemSGN, &m_SunBody);
			m_SunSGN.scale(Vector3f(0.7f, 0.7f, 0.7f));

			Quaternionf R;
			Quaternionf Ri;
			R = AngleAxisf(CForgeMath::degToRad(tRot / 120.0f), Vector3f::UnitY());
			Ri = AngleAxisf(CForgeMath::degToRad(tOrbit / 120.0f), Vector3f::UnitY());

			// Mercury
			m_MercurySGN.init(&m_SolarsystemSGN, &m_Mercury);
			m_MercurySGN.scale(Vector3f(0.1f, 0.1f, 0.1f));
			m_MercurySGN.position(CForgeMath::rotationMatrix(Ri) * Vector3f(2.0f, 2.0f, 0.0f));

				// rotation
			m_MercurySGN.rotation(R);
			
			// Venus
			m_VenusSGN.init(&m_SolarsystemSGN, &m_Venus);
			m_VenusSGN.scale(Vector3f(0.2f, 0.2f, 0.2f));
			m_VenusSGN.position(CForgeMath::rotationMatrix(Ri) * Vector3f(2.8f, 2.0f, 0.0f));

				// rotation
			m_VenusSGN.rotation(R);

			// Earth
			m_EarthSGN.init(&m_SolarsystemSGN, &m_Earth);
			m_EarthSGN.scale(Vector3f(0.2f, 0.2f, 0.2f));
			m_EarthSGN.position(CForgeMath::rotationMatrix(Ri) * Vector3f(3.6f, 2.0f, 0.0f));

				// rotation
			m_EarthSGN.rotation(R);

			// Mars
			m_MarsSGN.init(&m_SolarsystemSGN, &m_Mars);
			m_MarsSGN.scale(Vector3f(0.1f, 0.1f, 0.1f));
			m_MarsSGN.position(CForgeMath::rotationMatrix(Ri) * Vector3f(4.6f, 2.0f, 0.0f));

				// rotation
			m_MarsSGN.rotation(R);

			// Jupiter
			m_JupiterSGN.init(&m_SolarsystemSGN, &m_Jupiter);
			m_JupiterSGN.scale(Vector3f(0.4f, 0.4f, 0.4f));
			m_JupiterSGN.position(CForgeMath::rotationMatrix(Ri) * Vector3f(5.6f, 2.0f, 0.0f));

				// rotation
			m_JupiterSGN.rotation(R);

			// Saturn
			m_SaturnSGN.init(&m_SolarsystemSGN, &m_Saturn);
			m_SaturnSGN.scale(Vector3f(0.4f, 0.4f, 0.4f));
			m_SaturnSGN.position(CForgeMath::rotationMatrix(Ri) * Vector3f(6.6f, 2.0f, 0.0f));

				// rotation
			m_SaturnSGN.rotation(R);

			// Uranus
			m_UranusSGN.init(&m_SolarsystemSGN, &m_Uranus);
			m_UranusSGN.scale(Vector3f(0.3f, 0.3f, 0.3f));
			m_UranusSGN.position(CForgeMath::rotationMatrix(Ri) * Vector3f(7.6f, 2.0f, 0.0f));

				// rotation
			m_UranusSGN.rotation(R);

			// Neptune
			m_NeptuneSGN.init(&m_SolarsystemSGN, &m_Neptune);
			m_NeptuneSGN.scale(Vector3f(0.2f, 0.2f, 0.2f));
			m_NeptuneSGN.position(CForgeMath::rotationMatrix(Ri) * Vector3f(8.8f, 2.0f, 0.0f));

				// rotation
			m_NeptuneSGN.rotation(R);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) override {
			ExampleSceneBase::clear();
		}//clear

		void mainLoop(void) override {
			while (!m_RenderWin.shutdown()) {
				tRot = fmod(tRot + 45.0f, 360.0f);
				tOrbit += 45.0f;
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				//m_SkyboxSG.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_RenderDev.activeCamera((VirtualCamera*)m_Sun.camera());
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_RenderDev.activeCamera(&m_Cam);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);

				m_SkyboxSG.render(&m_RenderDev);

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

			}//while[main loop]
		}//run

	protected:

		// Scene Graph
		SceneGraph m_SkyboxSG;
		SGNTransformation m_SkyboxTransSGN;
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkyboxSGN;

		SGNTransformation m_SolarsystemSGN;

		SGNGeometry m_SunSGN;
		SGNGeometry m_MercurySGN;
		SGNGeometry m_VenusSGN;
		SGNGeometry m_EarthSGN;
		SGNGeometry m_MarsSGN;
		SGNGeometry m_JupiterSGN;
		SGNGeometry m_SaturnSGN;
		SGNGeometry m_UranusSGN;
		SGNGeometry m_NeptuneSGN;

		SkyboxActor m_Skybox;

		StaticActor m_SunBody;
		StaticActor m_Mercury;
		StaticActor m_Venus;
		StaticActor m_Earth;
		StaticActor m_Mars;
		StaticActor m_Jupiter;
		StaticActor m_Saturn;
		StaticActor m_Uranus;
		StaticActor m_Neptune;

		float tRot = 0.0f;
		float tOrbit = 0.0f;

	};//ExampleTransformation

}

#endif