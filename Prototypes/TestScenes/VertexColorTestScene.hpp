/*****************************************************************************\
*                                                                           *
* File(s): VertexColorTestScene.hpp                                            *
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
#ifndef __CFORGE_VERTEXCOLORTESTSCENE_HPP__
#define __CFORGE_VERTEXCOLORTESTSCENE_HPP__

//#include "../../CForge/AssetIO/SAssetIO.h"
//#include "../../CForge/Graphics/Shader/SShaderManager.h"
//#include "../../CForge/Graphics/STextureManager.h"
//
//#include "../../CForge/Graphics/GLWindow.h"
//#include "../../CForge/Graphics/GraphicsUtility.h"
//#include "../../CForge/Graphics/RenderDevice.h"
//
//
//#include "../../CForge/Graphics/Lights/DirectionalLight.h"
//#include "../../CForge/Graphics/Lights/PointLight.h"
//
//#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
//#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
//#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"
//
//#include "../../CForge/Graphics/Actors/StaticActor.h"
//
//#include "../../Examples/SceneUtilities.hpp"


#include "../../Examples/exampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class VertexColorTestScene : public ExampleSceneBase {
	public:
		VertexColorTestScene(void) {
			m_WindowTitle = "CForge - Vertex Color Test Scene";
		}//Constructor

		~VertexColorTestScene(void) {
			clear();
		}//Destructor

		void init(void) {
			initWindowAndRenderDevice();
			// initialize camera
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key light) and back ground light (fill light)
			Vector3f SunPos = Vector3f(25.0f, 25.0f, 75.0f);
			Vector3f SunLookAt = Vector3f(0.0f, 0.0f, 20.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
			m_Sun.init(SunPos, (SunLookAt - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			// sun will cast shadows
			m_Sun.initShadowCasting(1024*2, 1024*2, GraphicsUtility::orthographicProjection(40.0f, 40.0f, 1.0f, 1000.0f));
			m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.5f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			m_RenderDev.activeCamera(&m_Cam);
			m_RenderDev.addLight(&m_Sun);
			m_RenderDev.addLight(&m_BGLight);

			m_Cam.position(Vector3f(20.0f, 5.0f, 45.0f));
			m_Cam.lookAt(Vector3f(10.0f, 5.0f, 35.0f), Vector3f(0.0f, 4.0f, 25.0f), Vector3f::UnitY());

			ShaderCode::LightConfig LC;
			LC.DirLightCount = 1;
			LC.PointLightCount = 1;
			LC.SpotLightCount = 0;
			LC.PCFSize = 1;
			LC.ShadowBias = 0.0001f;
			LC.ShadowMapCount = 1;
			m_pShaderMan->configShader(LC);

			// load skydome and a textured cube
			T3DMesh<float> M;
			
			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			// initialize skeletal actor (Eric) and its animation controller
			T3DMesh<float> SkelAnim;
			SAssetIO::load("Assets/tmp/mbmalerigmusclessym.fbx", &M);
			SAssetIO::load("Assets/tmp/animation_scaled.fbx", &SkelAnim);
			//SAssetIO::load("MyAssets/animation_scaled.glb", &SkelAnim);
			//SAssetIO::load("MyAssets/Kniebeuge falsch-001.bvh", &SkelAnim);

			M.clearSkeletalAnimations();
			M.addSkeletalAnimation(SkelAnim.getSkeletalAnimation(0), true);

			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			m_Controller.init(&M);

			// assign random collor to material
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				Vector4f C;
				C.x() = CoreUtility::randRange<float>(0.0f, 1.0f);
				C.y() = CoreUtility::randRange<float>(0.0f, 1.0f);
				C.z() = CoreUtility::randRange<float>(0.0f, 1.0f);
				C.w() = 1.0f;
				M.getMaterial(i)->Color = C;
			}//for[materials]

			m_Eric.init(&M, &m_Controller);

			M.bones(nullptr, false);
			m_Human.init(&M);
			M.clear();

			// build scene graph
			
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome
			
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

			// add human model
			
			float HumanScale = 4.0f;
			m_HumanTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 1.0f, 0.0f), Quaternionf::Identity());
			//HumanSGN.init(&HumanTransformSGN, &Human);
			//HumanSGN.scale(Vector3f(HumanScale, HumanScale, HumanScale));

			// add skeletal actor to scene graph (Eric)
			
			m_EricTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 0.0f, 20.0f));
			m_EricSGN.init(&m_EricTransformSGN, &m_Eric);
			m_EricSGN.scale(Vector3f(HumanScale, HumanScale, HumanScale));
			Quaternionf R;
			R = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
			m_EricSGN.rotation(R);

			// check whether a GL error occurred
			std::string GLError = "";
			GraphicsUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

			m_LastColorChange = CoreUtility::timestamp();

			for (uint32_t i = 0; i < m_Eric.materialCount(); ++i) {
				Vector4f C1 = Vector4f(CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), 1.0f);
				Vector4f C2 = Vector4f(CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), 1.0f);
				m_Colors1.push_back(C1);
				m_Colors2.push_back(C2);
			}

		}//initialize

		void clear(void) {
			ExampleSceneBase::clear();
		}//clear

		void run(void) {

			m_FPS = 60.0f;
			m_FPSCount = 0;
			m_LastFPSPrint = CoreUtility::timestamp();

			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(m_FPS / 60.0f);

				// this will progress all active skeletal animations for this controller
				m_Controller.update(m_FPS / 60.0f);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				float AnimationSpeed = 1.0f; // 1000.0f / 24.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed = 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation* pAnim = m_Controller.createAnimation(0, AnimationSpeed, 0.0f);
					m_Eric.activeAnimation(pAnim);
				}


				// interpolate color
				float Alpha = float(CoreUtility::timestamp() - m_LastColorChange) / 2500.0f;
				for (uint32_t i = 0; i < m_Eric.materialCount(); ++i) {
					Vector4f C1 = m_Colors1[i];
					Vector4f C2 = m_Colors2[i];
					Vector4f C = (1.0f - Alpha) * C1 + Alpha * C2;
					m_Eric.material(i)->color(C);
				}//for[materials]


				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true) || (CoreUtility::timestamp() - m_LastColorChange) > 2500) {
					m_Colors1 = m_Colors2;
					m_Colors2.clear();

					for (uint32_t i = 0; i < m_Eric.materialCount(); ++i) {
						Vector4f C2 = Vector4f(CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), 1.0f);
						m_Colors2.push_back(C2);
					}//for[materials]
					m_LastColorChange = CoreUtility::timestamp();
				}



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

		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_HumanSGN;
		SGNTransformation m_HumanTransformSGN;

		StaticActor m_Skydome;
		SkeletalActor m_Eric;
		StaticActor m_Human;
		SkeletalAnimationController m_Controller;

		SGNGeometry m_EricSGN;
		SGNTransformation m_EricTransformSGN;

		std::vector<Vector4f> m_Colors1;
		std::vector<Vector4f> m_Colors2;
		uint64_t m_LastColorChange;
	};//VertexColorTestScene


	void vertexColorTestScene(void) {

		VertexColorTestScene Scene;
		Scene.init();
		Scene.run();
		Scene.clear();



		//SShaderManager* pSMan = SShaderManager::instance();

		//std::string WindowTitle = "CForge - Skeletal Animation Example";
		//float FPS = 60.0f;

		//bool const LowRes = false;

		//uint32_t WinWidth = 1280;
		//uint32_t WinHeight = 720;

		//if (LowRes) {
		//	WinWidth = 720;
		//	WinHeight = 576;
		//}

		//// create an OpenGL capable windows
		//GLWindow RenderWin;
		//RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), WindowTitle);

		//// configure and initialize rendering pipeline
		//RenderDevice RDev;
		//RenderDevice::RenderDeviceConfig Config;
		//Config.DirectionalLightsCount = 1;
		//Config.PointLightsCount = 1;
		//Config.SpotLightsCount = 0;
		//Config.ExecuteLightingPass = true;
		//Config.GBufferHeight = WinHeight;
		//Config.GBufferWidth = WinWidth;
		//Config.pAttachedWindow = &RenderWin;
		//Config.PhysicallyBasedShading = true;
		//Config.UseGBuffer = true;
		//RDev.init(&Config);

		//// configure and initialize shader configuration device
		//ShaderCode::LightConfig LC;
		//LC.DirLightCount = 1;
		//LC.PointLightCount = 1;
		//LC.SpotLightCount = 0;
		//LC.PCFSize = 0;
		//LC.ShadowBias = 0.00001f;
		//LC.ShadowMapCount = 1;
		//pSMan->configShader(LC);

		//// initialize camera
		//VirtualCamera Cam;
		//Cam.init(Vector3f(20.0f, 5.0f, 45.0f), Vector3f::UnitY());
		//Cam.lookAt(Vector3f(10.0f, 5.0f, 35.0f), Vector3f(0.0f, 4.0f, 25.0f), Vector3f::UnitY());
		//Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		//// initialize sun (key lights) and back ground light (fill light)
		//Vector3f SunPos = Vector3f(-25.0f, 50.0f, -20.0f);
		//Vector3f SunLookAt = Vector3f(0.0f, 0.0f, 30.0f);
		//Vector3f BGLightPos = Vector3f(0.0f, 5.0f, 60.0f);
		//DirectionalLight Sun;
		//PointLight BGLight;
		//Sun.init(SunPos, (SunLookAt - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
		//// sun will cast shadows
		//Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(20.0f, 20.0f, 0.1f, 1000.0f));
		//BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.5f, Vector3f(0.0f, 0.0f, 0.0f));

		//// set camera and lights
		//RDev.activeCamera(&Cam);
		//RDev.addLight(&Sun);
		//RDev.addLight(&BGLight);

		//// load skydome and a textured cube
		//T3DMesh<float> M;
		//StaticActor Skydome;
		//SkeletalActor Eric;
		//StaticActor Human;
		//SkeletalAnimationController Controller;

		//SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		//SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		//M.computePerVertexNormals();
		//Skydome.init(&M);
		//M.clear();

		//// initialize skeletal actor (Eric) and its animation controller
		//T3DMesh<float> SkelAnim;
		//SAssetIO::load("Assets/tmp/mbmalerigmusclessym.fbx", &M);
		////SAssetIO::load("Assets/tmp/animation_scaled.fbx", &SkelAnim);
		////SAssetIO::load("MyAssets/animation_scaled.glb", &SkelAnim);
		//SAssetIO::load("MyAssets/Kniebeuge falsch-001.bvh", &SkelAnim);



		//M.clearSkeletalAnimations();
		//M.addSkeletalAnimation(SkelAnim.getSkeletalAnimation(0), true);

		//SceneUtilities::setMeshShader(&M, 0.7f, 0.04f);
		//M.computePerVertexNormals();
		//Controller.init(&M);

		//// assign random collor to material
		//for (uint32_t i = 0; i < M.materialCount(); ++i) {
		//	Vector4f C;
		//	C.x() = CoreUtility::randRange<float>(0.0f, 1.0f);
		//	C.y() = CoreUtility::randRange<float>(0.0f, 1.0f);
		//	C.z() = CoreUtility::randRange<float>(0.0f, 1.0f);
		//	C.w() = 1.0f;
		//	M.getMaterial(i)->Color = C;
		//}//for[materials]

		//Eric.init(&M, &Controller);

		//M.bones(nullptr, false);
		//Human.init(&M);
		//M.clear();

		//// build scene graph
		//SceneGraph SG;
		//SGNTransformation RootSGN;
		//RootSGN.init(nullptr);
		//SG.init(&RootSGN);

		//// add skydome
		//SGNGeometry SkydomeSGN;
		//SkydomeSGN.init(&RootSGN, &Skydome);
		//SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

		//// add human model
		//SGNGeometry HumanSGN;
		//SGNTransformation HumanTransformSGN;
		//float HumanScale = 4.0f;
		//HumanTransformSGN.init(&RootSGN, Vector3f(0.0f, 1.0f, 0.0f), Quaternionf::Identity());
		////HumanSGN.init(&HumanTransformSGN, &Human);
		////HumanSGN.scale(Vector3f(HumanScale, HumanScale, HumanScale));

		//// add skeletal actor to scene graph (Eric)
		//SGNGeometry EricSGN;
		//SGNTransformation EricTransformSGN;
		//EricTransformSGN.init(&RootSGN, Vector3f(0.0f, 0.0f, 0.0f));
		//EricSGN.init(&EricTransformSGN, &Eric);
		//EricSGN.scale(Vector3f(HumanScale, HumanScale, HumanScale));
		//Quaternionf R;
		//R = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
		//EricSGN.rotation(R);

		//// stuff for performance monitoring
		//uint64_t LastFPSPrint = CoreUtility::timestamp();
		//int32_t FPSCount = 0;

		//// check wheter a GL error occurred
		//std::string GLError = "";
		//GraphicsUtility::checkGLError(&GLError);
		//if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		//uint64_t LastColorChange = CoreUtility::timestamp();
		//std::vector<Vector4f> Colors1;
		//std::vector<Vector4f> Colors2;
		//for (uint32_t i = 0; i < Eric.materialCount(); ++i) {
		//	Vector4f C1 = Vector4f(CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), 1.0f);
		//	Vector4f C2 = Vector4f(CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), 1.0f);
		//	Colors1.push_back(C1);
		//	Colors2.push_back(C2);
		//}
		//

		//// start main loop
		//while (!RenderWin.shutdown()) {
		//	RenderWin.update();
		//	SG.update(FPS / 60.0f);

		//	// this will progress all active skeletal animations for this controller
		//	Controller.update(FPS / 60.0f);

		//	SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());

		//	// if user hits key 1, animation will be played
		//	// if user also presses shift, animation speed is doubled
		//	float AnimationSpeed = 1.0f; // 1000.0f / 24.0f;
		//	if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed = 2.0f;
		//	if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
		//		SkeletalAnimationController::Animation* pAnim = Controller.createAnimation(0, AnimationSpeed, 0.0f);
		//		Eric.activeAnimation(pAnim);
		//	}


		//	// interpolate color
		//	float Alpha = float(CoreUtility::timestamp() - LastColorChange) / 2500.0f;
		//	for (uint32_t i = 0; i < Eric.materialCount(); ++i) {
		//		Vector4f C1 = Colors1[i];
		//		Vector4f C2 = Colors2[i];
		//		Vector4f C = (1.0f - Alpha) * C1 + Alpha * C2;
		//		Eric.material(i)->color(C);
		//	}//for[materials]


		//	if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true) || (CoreUtility::timestamp() - LastColorChange) > 2500) {
		//		Colors1 = Colors2;
		//		Colors2.clear();

		//		for (uint32_t i = 0; i < Eric.materialCount(); ++i) {	
		//			Vector4f C2 = Vector4f(CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), CoreUtility::randRange(0.0f, 1.0f), 1.0f);
		//			Colors2.push_back(C2);
		//		}//for[materials]
		//		LastColorChange = CoreUtility::timestamp();
		//	}
		//	


		//	RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
		//	SG.render(&RDev);

		//	RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
		//	SG.render(&RDev);

		//	RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

		//	RenderWin.swapBuffers();

		//	FPSCount++;
		//	if (CoreUtility::timestamp() - LastFPSPrint > 1000U) {
		//		char Buf[64];
		//		sprintf(Buf, "FPS: %d\n", FPSCount);
		//		FPS = float(FPSCount);
		//		FPSCount = 0;
		//		LastFPSPrint = CoreUtility::timestamp();

		//		RenderWin.title(WindowTitle + "[" + std::string(Buf) + "]");
		//	}

		//	if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
		//		RenderWin.closeWindow();
		//	}
		//}//while[main loop]

		//pSMan->release();

	}//exampleMinimumGraphicsSetup

}

#endif