/*****************************************************************************\
*                                                                           *
* File(s): exampleMultiViewport.hpp                                            *
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
#ifndef __CFORGE_EXAMPLEMULTIVIEWPORT_HPP__
#define __CFORGE_EXAMPLEMULTIVIEWPORT_HPP__

#include "../CForge/AssetIO/SAssetIO.h"
#include "../CForge/Graphics/Shader/SShaderManager.h"
#include "../CForge/Graphics/STextureManager.h"

#include "../CForge/Graphics/GLWindow.h"
#include "../CForge/Graphics/GraphicsUtility.h"
#include "../CForge/Graphics/RenderDevice.h"

#include "../CForge/Graphics/Lights/DirectionalLight.h"
#include "../CForge/Graphics/Lights/PointLight.h"

#include "../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../CForge/Graphics/Actors/StaticActor.h"

#include "SceneUtilities.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	void exampleMultiViewport(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge - Multi Viewport Example";
		float FPS = 60.0f;

		bool const LowRes = false;
		bool const HighRes = false;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		uint32_t GBufferWidth = 1280 / 2;
		uint32_t GBufferHeight = 720 / 2;

		if (LowRes) {
			WinWidth = 720;
			WinHeight = 576;
		}
		if (HighRes) {
			WinWidth = 1920;
			WinHeight = 1080;
		}



		// create an OpenGL capable windows
		GLWindow RenderWin;
		RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), WindowTitle);

		// configure and initialize rendering pipeline
		RenderDevice RDev;
		RenderDevice::RenderDeviceConfig Config;
		Config.DirectionalLightsCount = 1;
		Config.PointLightsCount = 1;
		Config.SpotLightsCount = 0;
		Config.ExecuteLightingPass = true;
		Config.GBufferHeight = GBufferHeight;
		Config.GBufferWidth = GBufferWidth;
		Config.pAttachedWindow = &RenderWin;
		Config.PhysicallyBasedShading = true;
		Config.UseGBuffer = true;
		RDev.init(&Config);

		// configure and initialize shader configuration device
		ShaderCode::LightConfig LC;
		LC.DirLightCount = 1;
		LC.PointLightCount = 1;
		LC.SpotLightCount = 0;
		LC.PCFSize = 1;
		LC.ShadowBias = 0.0004f;
		LC.ShadowMapCount = 1;
		pSMan->configShader(LC);

		// initialize camera
		VirtualCamera Cam;
		Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
		Cam.projectionMatrix(GBufferWidth, GBufferHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		// initialize sun (key lights) and back ground light (fill light)
		Vector3f SunPos = Vector3f(-5.0f, 15.0f, 35.0f);
		Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
		DirectionalLight Sun;
		PointLight BGLight;
		Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
		// sun will cast shadows
		Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(10.0f, 10.0f, 0.1f, 1000.0f));
		BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.5f, Vector3f(0.0f, 0.0f, 0.0f));

		// set camera and lights
		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);
		RDev.addLight(&BGLight);

		// load skydome and a textured cube
		T3DMesh<float> M;
		StaticActor Skydome;
		StaticActor Helmet;

		SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		M.computePerVertexNormals();
		Skydome.init(&M);
		M.clear();

		SAssetIO::load("Assets/ExampleScenes/Helmet/DamagedHelmet.gltf", &M);
		SceneUtilities::setMeshShader(&M, 0.2f, 0.24f);
		M.computePerVertexNormals();
		M.computePerVertexTangents();
		Helmet.init(&M);
		M.clear();

		// build scene graphs (one for every viewport)
		SceneGraph SGs[4];
		SGNTransformation RootSGNs[4];

		//SGNTransformation DomeTransSGNs[4];
		SGNTransformation HelmetTransSGNs[4];
		SGNGeometry DomeGeomSGNs[4];
		SGNGeometry HelmetGeomSGNs[4];

		float Speed = 10.0f; // Degree per second
		Quaternionf HelmetRotationDeltas[4];
		HelmetRotationDeltas[0] = AngleAxisf(GraphicsUtility::degToRad(Speed / 60.0f), Vector3f::UnitX());
		HelmetRotationDeltas[1] = AngleAxisf(GraphicsUtility::degToRad(Speed / 60.0f), Vector3f::UnitY());
		HelmetRotationDeltas[2] = AngleAxisf(GraphicsUtility::degToRad(Speed / 60.0f), Vector3f::UnitZ());
		HelmetRotationDeltas[3] = AngleAxisf(GraphicsUtility::degToRad(Speed / 60.0f), Vector3f(1.0f, 1.0f, 1.0f).normalized());

		for (uint8_t i = 0; i < 4; ++i) {
			RootSGNs[i].init(nullptr);
			SGs[i].init(&RootSGNs[i]);

			// add skydome
			DomeGeomSGNs[i].init(&RootSGNs[i], &Skydome, Vector3f::Zero(), Quaternionf::Identity(), Vector3f(5.0f, 5.0f, 5.0f));
			// add helmet
			HelmetTransSGNs[i].init(&RootSGNs[i], Vector3f(0.0f, 6.5f, 0.0f));
			HelmetGeomSGNs[i].init(&HelmetTransSGNs[i], &Helmet, Vector3f::Zero(), Quaternionf::Identity(), Vector3f(5.0f, 5.0f, 5.0f));

			// let the helmets spin
			HelmetTransSGNs[i].rotationDelta(HelmetRotationDeltas[i]);	
		}

		// we need one viewport for the GBuffer
		RenderDevice::Viewport GBufferVP;
		GBufferVP.Position = Vector2i(0, 0);
		GBufferVP.Size = Vector2i(GBufferWidth, GBufferHeight);
		
		
		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;

		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		// main rendering loop
		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());

			uint32_t RenderWinWidth = RenderWin.width();
			uint32_t RenderWinHeight = RenderWin.height();

			Cam.projectionMatrix(RenderWinWidth, RenderWinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

			uint32_t Margin = 14;
			Vector2i VPSize = Vector2i(RenderWinWidth / 2, RenderWinHeight / 2) - 2 * Vector2i(Margin, Margin) + Vector2i(Margin/2, Margin/2);

			RenderDevice::Viewport VPs[4];
			// Top left
			VPs[0].Position = Vector2i(0, RenderWinHeight / 2) + Vector2i(Margin, Margin/2);
			// top right
			VPs[1].Position = Vector2i(RenderWinWidth / 2, RenderWinHeight / 2) + Vector2i(Margin/2, Margin/2);
			// bottom left
			VPs[2].Position = Vector2i(Margin, Margin);
			// bottom right
			VPs[3].Position = Vector2i(RenderWinWidth / 2, 0) + Vector2i(Margin/2, Margin);
			for (uint8_t i = 0; i < 4; ++i) VPs[i].Size = VPSize;


			// perform rendering for the 4 viewports
			for (uint8_t i = 0; i < 4; ++i) {
				SGs[i].update(60.0f / FPS);

				// render scene as usual
				RDev.viewport(GBufferVP);
				RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
				SGs[i].render(&RDev);
				RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				SGs[i].render(&RDev);

				// set viewport and perform lighting pass
				// this will produce the correct tile in the final output window (backbuffer to be specific)
				RDev.viewport(VPs[i]);
				RDev.activePass(RenderDevice::RENDERPASS_LIGHTING, nullptr, (i==0) ? true : false);
			}



			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_F10, true)) {
				RenderDevice::Viewport V;
				V.Position = Vector2i(0, 0);
				V.Size = Vector2i(RenderWinWidth, RenderWinHeight);
				RDev.viewport(V);
				RDev.activePass(RenderDevice::RENDERPASS_FORWARD);
				SceneUtilities::takeScreenshot("Screenshot.jpg");
			}

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

	}//exampleMultiViewport

}

#endif