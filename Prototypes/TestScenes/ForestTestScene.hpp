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
#ifndef __CFORGE_FORESTTESTSCENE_HPP__
#define __CFORGE_FORESTTESTSCENE_HPP__

#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"
#include "../../CForge/Graphics/STextureManager.h"

#include "../../CForge/Graphics/GLWindow.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Graphics/RenderDevice.h"

#include "../../CForge/Graphics/Lights/DirectionalLight.h"
#include "../../CForge/Graphics/Lights/PointLight.h"

#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../../CForge/Graphics/Actors/StaticActor.h"

#include "../../Examples/SceneUtilities.hpp"

#include "../Actor/LODActor.h"
#include "../SLOD.h"

using namespace Eigen;
using namespace std;



namespace CForge {

	void forestTestScene(void) {
		SShaderManager* pSMan = SShaderManager::instance();
		SLOD* pLOD = SLOD::instance();

		std::string WindowTitle = "CForge - Skeletal Animation Example";
		float FPS = 60.0f;

		bool const LowRes = false;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		if (LowRes) {
			WinWidth = 720;
			WinHeight = 576;
		}
		pLOD->setResolution(Vector2i(WinWidth, WinHeight));

		// create an OpenGL capable windows
		GLWindow RenderWin;
		RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), WindowTitle);
		gladLoadGL();
		glfwSwapInterval(0);

		// configure and initialize rendering pipeline
		RenderDevice RDev;
		RenderDevice::RenderDeviceConfig Config;
		Config.DirectionalLightsCount = 1;
		Config.PointLightsCount = 1;
		Config.SpotLightsCount = 0;
		Config.ExecuteLightingPass = true;
		Config.GBufferHeight = WinHeight;
		Config.GBufferWidth = WinWidth;
		Config.pAttachedWindow = &RenderWin;
		Config.PhysicallyBasedShading = true;
		Config.UseGBuffer = true;
		RDev.init(&Config);

		// configure and initialize shader configuration device
		ShaderCode::LightConfig LC;
		LC.DirLightCount = 1;
		LC.PointLightCount = 1;
		LC.SpotLightCount = 0;
		LC.PCFSize = 0;
		LC.ShadowBias = 0.00001f;
		LC.ShadowMapCount = 1;
		pSMan->configShader(LC);

		// initialize camera
		VirtualCamera Cam;
		Cam.init(Vector3f(20.0f, 5.0f, 45.0f), Vector3f::UnitY());
		Cam.lookAt(Vector3f(10.0f, 5.0f, 35.0f), Vector3f(0.0f, 4.0f, 25.0f), Vector3f::UnitY());
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 800.0f);

		// initialize sun (key lights) and back ground light (fill light)
		Vector3f SunPos = Vector3f(-25.0f, 150.0f, 200.0f);
		Vector3f SunLookAt = Vector3f(0.0f, 0.0f, -30.0f);
		Vector3f BGLightPos = Vector3f(0.0f, 5.0f, 60.0f);
		DirectionalLight Sun;
		PointLight BGLight;
		Sun.init(SunPos, (SunLookAt - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 14.0f);
		// sun will cast shadows
		Sun.initShadowCasting(2048, 2048, GraphicsUtility::orthographicProjection(70.0f, 70.0f, 0.1f, 1000.0f));
		BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 13.5f, Vector3f(0.0f, 0.0f, 0.0f));

		// set camera and lights
		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);
		RDev.addLight(&BGLight);

		// load skydome and tree
		T3DMesh<float> M;
		StaticActor Skydome; //TODO compatibility with static actors / non instanced actors
		StaticActor Tree1;
		StaticActor Tree2;

		SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		M.computePerVertexNormals();
		//for (uint32_t i = 0; i < M.materialCount(); i++) {
		//	M.getMaterial(i)->VertexShaderSources[0] = "Shader/InstancedGeometryPass.vert";
		//}
		Skydome.init(&M);
		M.clear();

		
		AssetIO::load("Assets/tmp/lowpolytree.obj", &M);
		SceneUtilities::setMeshShader(&M, 0.7f, 0.94f);
		M.computePerVertexNormals();
		//for (uint32_t i = 0; i < M.materialCount(); i++) {
		//	M.getMaterial(i)->VertexShaderSources[0] = "Shader/InstancedGeometryPass.vert";
		//}
		Tree1.init(&M);
		M.clear();

		AssetIO::load("Assets/tmp/Lowpoly_tree_sample.obj", &M);
		SceneUtilities::setMeshShader(&M, 0.7f, 0.94f);
		M.computePerVertexNormals();
		//for (uint32_t i = 0; i < M.materialCount(); i++) {
		//	M.getMaterial(i)->VertexShaderSources[0] = "Shader/InstancedGeometryPass.vert";
		//}
		Tree2.init(&M);
		M.clear();

		// build scene graph
		SceneGraph SG;
		SGNTransformation RootSGN;
		RootSGN.init(nullptr);
		SG.init(&RootSGN);

		// add skydome
		SGNGeometry SkydomeSGN;
		SkydomeSGN.init(&RootSGN, &Skydome);
		SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

		// generate a forest
		const uint32_t TreeCount = 1000;
		SGNGeometry* pTreeNodes = new SGNGeometry[TreeCount];
		SGNTransformation* pTreeTransNodes = new SGNTransformation[TreeCount];

		float MinPlane = -100.0f;
		float MaxPlane = 100.0f;

		for (uint32_t i = 0; i < TreeCount; ++i) {

			// placement in world
			Vector3f Pos;
			Pos.x() = CoreUtility::randRange(MinPlane, MaxPlane);
			Pos.y() = 0.0f;
			Pos.z() = CoreUtility::randRange(MinPlane, MaxPlane);

			float Scaling = CoreUtility::randRange(0.8f, 1.4f);

			Quaternionf RotationY;
			RotationY = AngleAxisf(GraphicsUtility::degToRad(CoreUtility::randRange(0.0f, 360.0f)), Vector3f::UnitY());


			pTreeTransNodes[i].init(&RootSGN);
			pTreeTransNodes[i].translation(Pos);
			pTreeTransNodes[i].scale(Vector3f(Scaling, Scaling, Scaling));
			pTreeTransNodes[i].rotation(RotationY);

			
			if (CoreUtility::rand() % 5 != 0) {
				Vector3f StaticOffset = Vector3f(0.0f, 1.8f * Scaling, 0.0f);
				pTreeNodes[i].init(&pTreeTransNodes[i], &Tree1, StaticOffset);
			}
			else {
				Vector3f StaticOffset = Vector3f(0.0f, 0.0f * Scaling, 0.0f);
				Vector3f StaticScale = Vector3f(0.15f, 0.15f, 0.15f);
				pTreeNodes[i].init(&pTreeTransNodes[i], &Tree2, StaticOffset, Quaternionf::Identity(), StaticScale);
			}
			

		}//for[generate trees]


		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;

		// check wheter a GL error occurred
		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());


		// start main loop
		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SG.update(1.0f/pLOD->getDeltaTime());
			pLOD->update();
			RDev.clearBuffer();

			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());
			
			//RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
			//SG.render(&RDev);
			
			RDev.activePass(RenderDevice::RENDERPASS_LOD);
			SG.render(&RDev);
			
			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			//SG.render(&RDev);

			RDev.renderLODSG();

			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
			
			//RDev.activePass(RenderDevice::RENDERPASS_FORWARD);

			//if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_F2, true)) {
			//	static int32_t ScreenshotCount = 0;
			//	T2DImage<uint8_t> Img;
			//	T2DImage<uint8_t> DepthBuffer;
			//	GraphicsUtility::retrieveFrameBuffer(&Img, &DepthBuffer, 0.1f, 200.0f);
			//	AssetIO::store("../../Screenshot_" + std::to_string(ScreenshotCount) + ".jpg", &Img);
			//	AssetIO::store("../../DepthBuffer_" + std::to_string(ScreenshotCount) + ".jpg", &DepthBuffer);

			//	Img.clear();

			//	ScreenshotCount++;
			//}

			RenderWin.swapBuffers();
			

			FPSCount++;
			if (CoreUtility::timestamp() - LastFPSPrint > 1000U) {
				char Buf[64];
				sprintf(Buf, "FPS: %d\n", FPSCount);
				FPS = float(FPSCount);
				FPSCount = 0;
				LastFPSPrint = CoreUtility::timestamp();

				RenderWin.title(WindowTitle + "[" + std::string(Buf) + "]");
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
		}//while[main loop]

		pSMan->release();
		pLOD->release();
	}//exampleMinimumGraphicsSetup

}

#endif