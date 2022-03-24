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

#include "imgui.h"
#include "imgui_impl_glfw.h" // derzeit Fehler: siehe Zeile 92
#include "imgui_impl_opengl3.h"

#include "../TemplateRegistration/ArcBall.h"

using namespace Eigen;

namespace CForge {

	void processInput(const ImGuiIO& GuiIO);

	void rotateSplitViewGeometry(Mouse* pMouse, RenderDevice::Viewport& VP, SGNGeometry& GeomSGN, SGNTransformation& TransSGN, 
		TempReg::ArcBall& ArcBall, int32_t& FocussedView, uint8_t CurrentView);

	bool mouseInViewPort(Mouse* pMouse, RenderDevice::Viewport& VP);

	// TODO: Rotation based on mouse picking in 2 modes: rotate only picked model, rotate both models at once - Line 359 
	//void rotateCombinedViewGeometry(Mouse* pMouse, RenderDevice::Viewport& VP, /*...*/);

	void tempRegTestScene(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge - Template Registration Example";
		float FPS = 60.0f;

		bool const LowRes = false;
		bool const HighRes = true;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		//uint32_t GBufferWidth = 1280 / 2;
		//uint32_t GBufferHeight = 720 / 2;

		uint32_t GBufferWidth = 1600;
		uint32_t GBufferHeight = 900;

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




		// Dear ImGui Initialization
		// derzeit Fehler: Fehler C2664 "bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow *,bool)" : Konvertierung von Argument 1 von "CForge::GLFWwindow *" in "GLFWwindow *" nicht möglich
		// (zugehöriger Header von Dear ImGui: imgui_impl_glfw.h)
		// 
		//IMGUI_CHECKVERSION();
		//ImGui::CreateContext();
		//ImGuiIO& GuiIO = ImGui::GetIO();		
		//ImGui::StyleColorsLight();
		//ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)RenderWin.handle(), false); //install_callbacks = true setzen, wenn false probleme macht
		//ImGui_ImplOpenGL3_Init("#version 460 core");
		



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

		// load meshes
		//T3DMesh<float> SkydomeMesh;
		T3DMesh<float> TemplateMesh, TargetMesh;
		//StaticActor Skydome;
		StaticActor Template, Target;

		/*SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &SkydomeMesh);
		SceneUtilities::setMeshShader(&SkydomeMesh, 0.8f, 0.04f);
		SkydomeMesh.computePerVertexNormals();
		Skydome.init(&SkydomeMesh);
		SkydomeMesh.clear();*/

		SAssetIO::load("Assets/ExampleScenes/TempReg/Template.obj", &TemplateMesh);
		SceneUtilities::setMeshShader(&TemplateMesh, 0.4f, 0.0f);
		TemplateMesh.computePerVertexNormals();
		
		// assign default color (light orange)
		Vector3f C;
		C.x() = 1.0f;
		C.y() = 0.666f;
		C.z() = 0.498f;

		std::vector<Eigen::Vector3f> Colors;
		for (uint32_t i = 0; i < TemplateMesh.vertexCount(); i++) Colors.push_back(C);
		TemplateMesh.colors(&Colors);

		Template.init(&TemplateMesh);

		SAssetIO::load("Assets/ExampleScenes/TempReg/Template.obj", &TargetMesh);
		SceneUtilities::setMeshShader(&TargetMesh, 0.4f, 0.0f);
		TargetMesh.computePerVertexNormals();
		
		//assign default color (light blue)
		C.x() = 0.541f;
		C.y() = 0.784f;
		C.z() = 1.0f;

		Colors.clear();
		for (uint32_t i = 0; i < TargetMesh.vertexCount(); i++) Colors.push_back(C);
		TargetMesh.colors(&Colors);

		Target.init(&TargetMesh);

		// build scene graphs
		SceneGraph SplitViewSGs[2], CombinedViewSG;
		SGNTransformation SplitViewRootSGNs[2], CombinedViewRootSGN;

		// template and target nodes
		SGNTransformation SplitViewTransSGNs[2], CombinedViewTransSGN[2];
		SGNGeometry SplitViewGeomSGNs[2], CombinedViewGeomSGN[2];
		
		// initialize split view scene graphs
		for (uint8_t i = 0; i < 2; ++i) {
			SplitViewRootSGNs[i].init(nullptr);
			SplitViewSGs[i].init(&SplitViewRootSGNs[i]);
		}

		// => template
		SplitViewTransSGNs[0].init(&SplitViewRootSGNs[0], Vector3f(0.0f, 0.0f, 0.0f));
		SplitViewGeomSGNs[0].init(&SplitViewTransSGNs[0], &Template, Vector3f::Zero(), Quaternionf::Identity());
		// => target
		SplitViewTransSGNs[1].init(&SplitViewRootSGNs[1], Vector3f(0.0f, 0.0f, 0.0f));
		SplitViewGeomSGNs[1].init(&SplitViewTransSGNs[1], &Target, Vector3f::Zero(), Quaternionf::Identity());

		// initialize combined view scene graph
		CombinedViewRootSGN.init(nullptr);
		CombinedViewSG.init(&CombinedViewRootSGN);
		
		// => template
		CombinedViewTransSGN[0].init(&CombinedViewRootSGN, Vector3f(0.0f, 0.0f, 0.0f));
		CombinedViewGeomSGN[0].init(&CombinedViewTransSGN[0], &Template, Vector3f::Zero(), Quaternionf::Identity());
		// => target
		CombinedViewTransSGN[1].init(&CombinedViewRootSGN, Vector3f(0.0f, 0.0f, 0.0f));
		CombinedViewGeomSGN[1].init(&CombinedViewTransSGN[1], &Target, Vector3f::Zero(), Quaternionf::Identity());

		TempReg::ArcBall ArcBalls[4];

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
		int32_t FocussedView = -1;

		while (!RenderWin.shutdown()) {
			RenderWin.update();

			// handle user input for gui and viewports
			//processInput(GuiIO);

			// ready new gui frame
			//ImGui_ImplOpenGL3_NewFrame();
			//ImGui_ImplGlfw_NewFrame();
			//ImGui::NewFrame();

			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());

			uint32_t RenderWinWidth = RenderWin.width();
			uint32_t RenderWinHeight = RenderWin.height();

			

			//uint32_t Margin = 6;

			// split views
			//Vector2i VPSize = Vector2i(RenderWinWidth / 2, RenderWinHeight) - 2 * Vector2i(Margin, Margin) + Vector2i(Margin / 2, Margin / 2);
			Vector2i VPSize = Vector2i(RenderWinWidth / 2, RenderWinHeight);
			RenderDevice::Viewport SplitVPs[2];
			// left split view
			//SplitVPs[0].Position = Vector2i(Margin, Margin);
			SplitVPs[0].Position = Vector2i(0, 0);
			//right split view
			//SplitVPs[1].Position = Vector2i(RenderWinWidth / 2, 0) + Vector2i(Margin / 2, Margin);
			SplitVPs[1].Position = Vector2i(RenderWinWidth / 2, 0);
			for (uint8_t i = 0; i < 2; ++i) SplitVPs[i].Size = VPSize;

			Cam.projectionMatrix(VPSize.x(), VPSize.y(), GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

			// combined view
			/*VPSize = Vector2i(RenderWinWidth, RenderWinHeight);
			RenderDevice::Viewport CombinedVP;
			CombinedVP.Position = Vector2i(0, 0);
			CombinedVP.Size = VPSize;*/

			// render split views (TEST)
			for (uint8_t i = 0; i < 2; ++i) {
				rotateSplitViewGeometry(RenderWin.mouse(), SplitVPs[i], SplitViewGeomSGNs[i], SplitViewTransSGNs[i], ArcBalls[i], FocussedView, i);

				SplitViewSGs[i].update(60.0f / FPS);

				// render scene as usual
				RDev.viewport(GBufferVP);
				RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
				SplitViewSGs[i].render(&RDev);
				RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				SplitViewSGs[i].render(&RDev);

				// set viewport and perform lighting pass
				// this will produce the correct tile in the final output window (backbuffer to be specific)
				RDev.viewport(SplitVPs[i]);
				RDev.activePass(RenderDevice::RENDERPASS_LIGHTING, nullptr, (i == 0) ? true : false);
			}

			// render combined view (TEST)
			/*SGs[2].update(60.0f / FPS);
			RDev.viewport(GBufferVP);
			RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
			SGs[2].render(&RDev);
			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			SGs[2].render(&RDev);
			RDev.viewport(SplitVPs[2]);
			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING, nullptr, false);*/



			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_F10, true)) {
				RenderDevice::Viewport V;
				V.Position = Vector2i(0, 0);
				V.Size = Vector2i(RenderWinWidth, RenderWinHeight);
				RDev.viewport(V);
				RDev.activePass(RenderDevice::RENDERPASS_FORWARD);
				SceneUtilities::takeScreenshot("Screenshot.jpg");
			}

			// render gui
			//ImGui::Render();
			//glViewport(0, 0, RenderWinWidth, RenderWinHeight);
			//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

	void processInput(const ImGuiIO& GuiIO) {

		// only forward keyboard input data to viewport controls if not captured by gui
		// WantCaptureKeyboard-flag updated by ImGui::NewFrame(): calling processInput(...) before calling NewFrame() to poll flags from previous frame allows for more reliable input processing
		// (as per: https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-tell-whether-to-dispatch-mousekeyboard-to-dear-imgui-or-my-application)
		
		if (!GuiIO.WantCaptureKeyboard) {
			//...
		}

		// only forward mouse input data to viewport controls if not captured by gui 
		// WantCaptureMouse-flag updated by ImGui::NewFrame(): calling processInput(...) before calling NewFrame() to poll flags from previous frame allows for more reliable input processing
		// (as per: https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-tell-whether-to-dispatch-mousekeyboard-to-dear-imgui-or-my-application)
		
		if (!GuiIO.WantCaptureMouse) {
			//...
		}
	}

	bool mouseInViewPort(Mouse* pMouse, RenderDevice::Viewport& VP) {
		if ((pMouse->position().x() >= VP.Position.x() && pMouse->position().y() >= VP.Position.y()) &&
			(pMouse->position().x() <= VP.Position.x() + VP.Size.x() && pMouse->position().y() <= VP.Position.y() + VP.Size.y())) {
			return true;
		}

		return false;
	}

	void rotateSplitViewGeometry(Mouse* pMouse, RenderDevice::Viewport& VP, SGNGeometry& GeomSGN, SGNTransformation& TransSGN, 
		TempReg::ArcBall& ArcBall, int32_t& FocussedView, uint8_t CurrentView) {
		
		if (pMouse->buttonState(Mouse::BTN_LEFT)) {
			if (FocussedView == -1 && mouseInViewPort(pMouse, VP)) {

					FocussedView = CurrentView;
					ArcBall.startRotation(pMouse->position().x(), pMouse->position().y(), VP.Size);
			}
			
			if (FocussedView == CurrentView) {
				TransSGN.rotation(ArcBall.updateRotation(pMouse->position().x(), pMouse->position().y(), VP.Size) * TransSGN.rotation());
			}
		}
		else if (FocussedView != -1) FocussedView = -1;
	}

	//void rotateCombinedViewGeometry(Mouse* pMouse, RenderDevice::Viewport& VP, /*...*/); // TODO: Rotation based on mouse picking in 2 modes: rotate only picked model, rotate both models at once
}



#endif