#ifndef __CFORGE_TEMPLATEREGISTRATIONTESTSCENE_HPP__
#define __CFORGE_TEMPLATEREGISTRATIONTESTSCENE_HPP__

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

#include "../TemplateRegistration/TempRegAppState.h"
#include "../TemplateRegistration/ViewportRenderManager.h"
#include "../TemplateRegistration/ViewportInputManager.h"
#include "../TemplateRegistration/GUIManager.h"
#include "../TemplateRegistration/Dataset/DatasetCommon.h"
#include "../TemplateRegistration/Dataset/DatasetGeometryData.h"

using namespace Eigen;
using namespace TempReg;

namespace CForge {

	void tempRegTestScene(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge - Template Registration Example";
		float FPS = 60.0f;

		bool const LowRes = false;
		bool const HighRes = true;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		uint32_t GBufferWidth = 1024;
		uint32_t GBufferHeight = 768;

		//uint32_t GBufferWidth = 1600;
		//uint32_t GBufferHeight = 900;

		if (LowRes) {
			WinWidth = 720;
			WinHeight = 576;
		}
		if (HighRes) {
			WinWidth = 1600;
			WinHeight = 900;
		}

		// create an OpenGL capable windows
		GLWindow RenderWin;
		RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), WindowTitle);

		gladLoadGL();

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
 
		// initialize GUI
		GUIManager GUIMgr;
		GUIMgr.init(RenderWin.handle(), RenderWin.width(), RenderWin.height());
		
		// global application states (template registration app)
		TempRegAppState GlobalAppState;
		
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
		VirtualCamera DummyCam;
		DummyCam.init(Vector3f(0.0f, 0.0f, 15.0f), Vector3f::UnitY());
		DummyCam.projectionMatrix(GBufferWidth, GBufferHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

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
		RDev.activeCamera(&DummyCam);
		RDev.addLight(&Sun);
		RDev.addLight(&BGLight);

		// load dataset geometry

		//std::map<DatasetType, DatasetGeometryData*> DatasetGeometries; 
		std::map<DatasetType, DatasetGeometryData> DatasetGeometries;//TODO: move to better spot!
		DatasetGeometries.try_emplace(DatasetType::TEMPLATE); //TODO: move to GUI!
		DatasetGeometries.at(DatasetType::TEMPLATE).initFromFile("Assets/ExampleScenes/TempReg/Template.obj", DatasetGeometryType::MESH);
		DatasetGeometries.at(DatasetType::TEMPLATE).initMeshAdjacencyData();
		DatasetGeometries.try_emplace(DatasetType::DTEMPLATE); //TODO: move to GUI!
		DatasetGeometries.at(DatasetType::DTEMPLATE).initFromFile("Assets/ExampleScenes/TempReg/Template.obj", DatasetGeometryType::MESH);
		DatasetGeometries.at(DatasetType::DTEMPLATE).initMeshAdjacencyData();
		DatasetGeometries.try_emplace(DatasetType::TARGET); //TODO: move to GUI! --- ".../Template.obj" used for testing purposes, change to Target.obj!
		DatasetGeometries.at(DatasetType::TARGET).initFromFile("Assets/ExampleScenes/TempReg/Template.obj", DatasetGeometryType::MESH);
		DatasetGeometries.at(DatasetType::TARGET).initMeshAdjacencyData();

		CorrespondenceStorage CorrStorage(DatasetGeometries.at(DatasetType::TEMPLATE).vertexCount(), DatasetGeometries.at(DatasetType::TARGET).vertexCount());

		// init ViewportManager: viewports + scenegraphs, render data
		ViewportRenderManager VPRMgr(4);
		VPRMgr.initMarkerData("Assets/ExampleScenes/TempReg/UnitSphere.obj");
		VPRMgr.initDatasetModelFromFile(DatasetType::TEMPLATE, "Assets/ExampleScenes/TempReg/Template.obj"); //TODO: move to GUI!
		VPRMgr.initDatasetModelFromFile(DatasetType::DTEMPLATE, "Assets/ExampleScenes/TempReg/Template.obj"); //TODO: move to GUI!
		VPRMgr.initDatasetModelFromFile(DatasetType::TARGET, "Assets/ExampleScenes/TempReg/Template.obj"); //TODO: move to GUI! --- ".../Template.obj" used for testing purposes, change to Target.obj!

		ViewportInputManager VPIMgr;

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
			RenderWin.update(); // calls glfwPollEvents()
			uint32_t RenderWinWidth = RenderWin.width();
			uint32_t RenderWinHeight = RenderWin.height();

			// handle user input for gui and viewports
			VPIMgr.processInput(RenderWin, GlobalAppState, GUIMgr, VPRMgr, DatasetGeometries, CorrStorage);

			// ready new Dear ImGui frame
			GUIMgr.buildNextImGuiFrame(GlobalAppState, VPRMgr, DatasetGeometries);
						
			// render views
			bool ClearBuffer = true;
			for (size_t ID = 0; ID < VPRMgr.activeViewportCount(); ++ID) {
				
				// update viewport position and dimension, camera for viewport, scene graph
				RDev.activeCamera(VPRMgr.viewportCam(ID));
				VPRMgr.updateViewport(ID, 60.0f / FPS);

				// render scene as usual
				RDev.viewport(GBufferVP);
				RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
				VPRMgr.renderViewport(ID, RDev);
				RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				VPRMgr.renderViewport(ID, RDev);

				// set viewport and perform lighting pass
				// this will produce the correct tile in the final output window (backbuffer to be specific)
				RDev.viewport(VPRMgr.viewportGetRenderDeviceViewport(ID));
				RDev.activePass(RenderDevice::RENDERPASS_LIGHTING, nullptr, ClearBuffer);
				ClearBuffer = false;
			}//for[render views]

			RDev.activeCamera(&DummyCam);

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_F10, true)) {
				RenderDevice::Viewport V;
				V.Position = Vector2i(0, 0);
				V.Size = Vector2i(RenderWinWidth, RenderWinHeight);
				RDev.viewport(V);
				RDev.activePass(RenderDevice::RENDERPASS_FORWARD);
				SceneUtilities::takeScreenshot("Screenshot.jpg");
			}

			// render gui
			GUIMgr.renderImGuiFrame(ClearBuffer);
			
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

	} //tempRegTestScene

	//void processInput(GLWindow& RenderWin, TempRegAppState& GlobalAppState, GUIManager& GUIMgr, ViewportManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries) {

	//}//processInput

	// raycast against datasets (for picking / highlighting of vertices) if mouse cursor hovers over a viewport
	// only do raycast when: 
	// a) exactly one dataset is active in given viewport or
	// b) mutliple datasets are displayed side by side (NOT layered over each other)
	//void raycastDatasets(
	//	
	//}//raycastDatasets

	//void pickPointOnMesh(
	//	
	//}//pickPointOnMesh

	//void selectDatasetComponentMesh(
	//	
	//}//selectDatasetComponentMesh
}

#endif