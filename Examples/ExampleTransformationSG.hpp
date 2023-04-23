/*****************************************************************************\
*                                                                           *
* File(s): ExampleTransformationSG.hpp                                     *
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
#ifndef __CFORGE_EXAMPLETRANSFORMATIONSG_HPP__
#define __CFORGE_EXAMPLETRANSFORMATIONSG_HPP__


#include "ExampleSceneBase.hpp"
#include "../CForge/Graphics/Actors/SkyboxActor.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleTransformationSG : public ExampleSceneBase {
	public:
		ExampleTransformationSG(void) {
			m_WindowTitle = "CrossForge Example - Transformation";
		}//Constructor

		~ExampleTransformationSG(void) {
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

			SAssetIO::load("Assets/ExampleScenes/Solar/moon/scene.gltf", &M);
			setMeshShader(&M, 0.4f, 0.02f);
			M.computePerVertexNormals();
			m_Moon.init(&M);
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
			m_SolarsystemSGN.init(&m_RootSGN, Vector3f(0.0f, 2.0f, 0.0f));

			//Sun
			m_SunTransformSGN.init(&m_SolarsystemSGN);
			m_SunSGN.init(&m_SunTransformSGN, &m_SunBody);
			m_SunSGN.scale(Vector3f(0.7f, 0.7f, 0.7f));

			// rotation
			R = AngleAxisf(CForgeMath::degToRad(20.0f / 120.0f), Vector3f::UnitY());
			m_SunTransformSGN.rotationDelta(R);

			// Mercury
			m_MercuryOrbitSGN.init(&m_SolarsystemSGN);
			m_MercuryTransformSGN.init(&m_MercuryOrbitSGN, Vector3f(2.0f, 0.0f, 0.0f));
			m_MercurySGN.init(&m_MercuryTransformSGN, &m_Mercury);
			m_MercurySGN.scale(Vector3f(0.1f, 0.1f, 0.1f));

				// orbiting
			Quaternionf R;
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_MercuryOrbitSGN.rotationDelta(R);

				// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_MercuryTransformSGN.rotationDelta(R);
			
			// Venus
			m_VenusOrbitSGN.init(&m_SolarsystemSGN);
			m_VenusTransformSGN.init(&m_VenusOrbitSGN, Vector3f(2.8f, 0.0f, 0.0f));
			m_VenusSGN.init(&m_VenusTransformSGN, &m_Venus);
			m_VenusSGN.scale(Vector3f(0.2f, 0.2f, 0.2f));

				// orbiting
			R = AngleAxisf(CForgeMath::degToRad(42.0f / 120.0f), Vector3f::UnitY());
			m_VenusOrbitSGN.rotationDelta(R);

				// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_VenusTransformSGN.rotationDelta(R);

			// Earth
			m_EarthOrbitSGN.init(&m_SolarsystemSGN);
			m_EarthTranslateSGN.init(&m_EarthOrbitSGN, Vector3f(3.6f, 0.0f, 0.0f));
			m_EarthRotationSGN.init(&m_EarthTranslateSGN);
			m_EarthSGN.init(&m_EarthRotationSGN, &m_Earth);
			m_EarthSGN.scale(Vector3f(0.2f, 0.2f, 0.2f));

				// orbiting
			R = AngleAxisf(CForgeMath::degToRad(40.0f / 120.0f), Vector3f::UnitY());
			m_EarthOrbitSGN.rotationDelta(R);

				// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_EarthRotationSGN.rotationDelta(R);

			// Moon
			m_MoonOrbitSGN.init(&m_EarthTranslateSGN);
			m_MoonTransformSGN.init(&m_MoonOrbitSGN, Vector3f(0.3f, 0.0f, 0.0f));
			m_MoonSGN.init(&m_MoonTransformSGN, &m_Moon);
			m_MoonSGN.scale(Vector3f(0.04f, 0.04f, 0.04f));

				// orbiting
			R = AngleAxisf(CForgeMath::degToRad(120.0f / 120.0f), Vector3f::UnitY());
			m_MoonOrbitSGN.rotationDelta(R);


			// Mars
			m_MarsOrbitSGN.init(&m_SolarsystemSGN);
			m_MarsTransformSGN.init(&m_MarsOrbitSGN, Vector3f(4.6f, 0.0f, 0.0f));
			m_MarsSGN.init(&m_MarsTransformSGN, &m_Mars);
			m_MarsSGN.scale(Vector3f(0.1f, 0.1f, 0.1f));

				// orbiting
			R = AngleAxisf(CForgeMath::degToRad(36.0f / 120.0f), Vector3f::UnitY());
			m_MarsOrbitSGN.rotationDelta(R);

				// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_MarsTransformSGN.rotationDelta(R);

			// Jupiter
			m_JupiterOrbitSGN.init(&m_SolarsystemSGN);
			m_JupiterTransformSGN.init(&m_JupiterOrbitSGN, Vector3f(5.6f, 0.0f, 0.0f));
			m_JupiterSGN.init(&m_JupiterTransformSGN, &m_Jupiter);
			m_JupiterSGN.scale(Vector3f(0.4f, 0.4f, 0.4f));

				// orbiting
			R = AngleAxisf(CForgeMath::degToRad(30.0f / 120.0f), Vector3f::UnitY());
			m_JupiterOrbitSGN.rotationDelta(R);

				// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_JupiterTransformSGN.rotationDelta(R);

			// Saturn
			m_SaturnOrbitSGN.init(&m_SolarsystemSGN);
			m_SaturnTransformSGN.init(&m_SaturnOrbitSGN, Vector3f(6.6f, 0.0f, 0.0f));
			m_SaturnSGN.init(&m_SaturnTransformSGN, &m_Saturn);
			m_SaturnSGN.scale(Vector3f(0.4f, 0.4f, 0.4f));

				// orbiting
			R = AngleAxisf(CForgeMath::degToRad(27.0f / 120.0f), Vector3f::UnitY());
			m_SaturnOrbitSGN.rotationDelta(R);

				// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_SaturnTransformSGN.rotationDelta(R);

			// Uranus
			m_UranusOrbitSGN.init(&m_SolarsystemSGN);
			m_UranusTransformSGN.init(&m_UranusOrbitSGN, Vector3f(7.6f, 0.0f, 0.0f));
			m_UranusSGN.init(&m_UranusTransformSGN, &m_Uranus);
			m_UranusSGN.scale(Vector3f(0.3f, 0.3f, 0.3f));

				// orbiting
			R = AngleAxisf(CForgeMath::degToRad(22.0f / 120.0f), Vector3f::UnitY());
			m_UranusOrbitSGN.rotationDelta(R);

				// rotation
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 120.0f), Vector3f::UnitY());
			m_UranusTransformSGN.rotationDelta(R);

			// Neptune
			m_NeptuneOrbitSGN.init(&m_SolarsystemSGN);
			m_NeptuneTransformSGN.init(&m_NeptuneOrbitSGN, Vector3f(8.8f, 0.0f, 0.0f));
			m_NeptuneSGN.init(&m_NeptuneTransformSGN, &m_Neptune);
			m_NeptuneSGN.scale(Vector3f(0.2f, 0.2f, 0.2f));

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

		void clear(void) override {
			ExampleSceneBase::clear();
		}//clear

		void mainLoop(void) override {
			while (!m_RenderWin.shutdown()) {
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
		SGNTransformation m_SunTransformSGN;
		SGNGeometry m_MercurySGN;
		SGNTransformation m_MercuryOrbitSGN;
		SGNTransformation m_MercuryTransformSGN;
		SGNGeometry m_VenusSGN;
		SGNTransformation m_VenusOrbitSGN;
		SGNTransformation m_VenusTransformSGN;
		SGNGeometry m_EarthSGN;
		SGNTransformation m_EarthOrbitSGN;
		SGNTransformation m_EarthRotationSGN;
		SGNTransformation m_EarthTranslateSGN;
		SGNGeometry m_MoonSGN;
		SGNTransformation m_MoonTransformSGN;
		SGNTransformation m_MoonOrbitSGN;
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

		SkyboxActor m_Skybox;

		StaticActor m_SunBody;
		StaticActor m_Mercury;
		StaticActor m_Venus;
		StaticActor m_Earth;
		StaticActor m_Moon;
		StaticActor m_Mars;
		StaticActor m_Jupiter;
		StaticActor m_Saturn;
		StaticActor m_Uranus;
		StaticActor m_Neptune;

	};//ExampleTransformationSG

}

#endif