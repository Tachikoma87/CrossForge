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

//#include "initImGUI.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <igl/embree/unproject_onto_mesh.h>

#include "../TemplateRegistration/TempRegAppState.h"
#include "../TemplateRegistration/Dataset.h"
#include "../TemplateRegistration/ViewportManager.h"
#include "../TemplateRegistration/GUIManager.h"

using namespace Eigen;
using namespace TempReg;

namespace CForge {

	void processInput(GLWindow* pRenderWin, TempRegAppState* pGlobalAppState, GUIManager* pGUIMgr, ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>& pDatasetGeometries);

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

		std::map<DatasetType, DatasetGeometryData*> DatasetGeometries; //TODO: move to better spot!

		DatasetGeometries.insert(std::pair<DatasetType, DatasetGeometryData*>(DatasetType::TEMPLATE, nullptr)); //TODO: move to GUI!
		DatasetGeometries.at(DatasetType::TEMPLATE) = new DatasetGeometryData();
		DatasetGeometries.at(DatasetType::TEMPLATE)->initFromFile("Assets/ExampleScenes/TempReg/Template.obj", DatasetGeometryType::MESH); //TODO: move to GUI!

		DatasetGeometries.insert(std::pair<DatasetType, DatasetGeometryData*>(DatasetType::DTEMPLATE, nullptr)); //TODO: move to GUI!
		DatasetGeometries.at(DatasetType::DTEMPLATE) = new DatasetGeometryData();
		DatasetGeometries.at(DatasetType::DTEMPLATE)->initFromFile("Assets/ExampleScenes/TempReg/Template.obj", DatasetGeometryType::MESH); //TODO: move to GUI!

		DatasetGeometries.insert(std::pair<DatasetType, DatasetGeometryData*>(DatasetType::TARGET, nullptr)); //TODO: move to GUI!
		DatasetGeometries.at(DatasetType::TARGET) = new DatasetGeometryData();
		DatasetGeometries.at(DatasetType::TARGET)->initFromFile("Assets/ExampleScenes/TempReg/Template.obj", DatasetGeometryType::MESH); //TODO: move to GUI! --- ".../Template.obj" used for testing purposes, change to Target.obj!

		// init ViewportManager: viewports + scenegraphs, render data
		ViewportManager VPMgr(4);
		VPMgr.initVertexMarkerModels("Assets/ExampleScenes/TempReg/UnitSphere.obj");
		VPMgr.initDatasetModelFromFile(DatasetType::TEMPLATE, "Assets/ExampleScenes/TempReg/Template.obj"); //TODO: move to GUI!
		VPMgr.initDatasetModelFromFile(DatasetType::DTEMPLATE, "Assets/ExampleScenes/TempReg/Template.obj"); //TODO: move to GUI!
		VPMgr.initDatasetModelFromFile(DatasetType::TARGET, "Assets/ExampleScenes/TempReg/Template.obj"); //TODO: move to GUI! --- ".../Template.obj" used for testing purposes, change to Target.obj!

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
			processInput(&RenderWin, &GlobalAppState, &GUIMgr, &VPMgr, DatasetGeometries);

			// ready new Dear ImGui frame
			GUIMgr.buildNextImGuiFrame(&GlobalAppState, &VPMgr, &DatasetGeometries);
						
			// render views
			bool ClearBuffer = true;
			for (size_t ID = 0; ID < VPMgr.activeViewportCount(); ++ID) {
				
				// update viewport position and dimension, camera for viewport, scene graph
				RDev.activeCamera(VPMgr.viewportCam(ID));
				VPMgr.updateViewport(ID, 60.0f / FPS);

				// render scene as usual
				RDev.viewport(GBufferVP);
				RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
				VPMgr.renderViewport(ID, &RDev);
				RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				VPMgr.renderViewport(ID, &RDev);

				// set viewport and perform lighting pass
				// this will produce the correct tile in the final output window (backbuffer to be specific)
				RDev.viewport(VPMgr.viewportGetRenderDeviceViewport(ID));
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

	void processInput(GLWindow* pRenderWin, TempRegAppState* pGlobalAppState, GUIManager* pGUIMgr, ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>& pDatasetGeometries) {

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// TODO: SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse()); //TODO disable WASD movement later, change to orbit camera (lookAt)
		// 
		// -> split between sections !GuiIO.WantCaptureKeyboard and !GuiIO.WantCaptureMouse!
		// -> execute updates per viewport!
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		ImGuiIO& GuiIO = ImGui::GetIO();

		// only forward keyboard input data to viewport controls if not captured by gui
		// WantCaptureKeyboard-flag updated by ImGui::NewFrame(): calling processInput(...) before calling NewFrame() to poll flags from previous frame allows for more reliable input processing
		// (as per: https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-tell-whether-to-dispatch-mousekeyboard-to-dear-imgui-or-my-application)
		
		if (!GuiIO.WantCaptureKeyboard) {
			//...
		}

		// only forward mouse input data to viewport controls if not captured by gui 
		// WantCaptureMouse-flag updated by ImGui::NewFrame(): calling processInput(...) before calling NewFrame() to poll flags from previous frame allows for more reliable input processing
		// (as per: https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-tell-whether-to-dispatch-mousekeyboard-to-dear-imgui-or-my-application)
		
		if (!GuiIO.WantCaptureMouse) { // mouse cursor is inside of viewport section area
			Vector2f CurrentCursorPosOGL = Vector2f(pRenderWin->mouse()->position().x(), (float)pRenderWin->height() - pRenderWin->mouse()->position().y());

			// current position of mouse cursor in relation to active viewports; if ViewportUnderMouse == -1 the cursor does not hover over any active viewport
			int32_t ViewportUnderMouse = pVPMgr->mouseInViewport(CurrentCursorPosOGL);

			// raycast against datasets (for picking / highlighting of vertices) if mouse cursor hovers over a viewport
			// raycast vertex under mouse cursor only when: a) exactly one dataset is active in a viewport or
			//										        b) mutliple datasets are displayed side by side in a viewport (NOT layered over each other)
			if (ViewportUnderMouse > -1) {				
				// get necessary states of viewport under mouse
				auto ActiveDatasets = pVPMgr->activeDatasetTypes(ViewportUnderMouse);
				auto ActiveDisplayDataArrangement = pVPMgr->activeDatasetDisplayDataArrangement(ViewportUnderMouse);
				
				Matrix4f View = pVPMgr->viewportViewMatrix(ViewportUnderMouse);
				Matrix4f Projection = pVPMgr->viewportProjectionMatrix(ViewportUnderMouse);
				
				Vector4f VP = Vector4f( // viewport dimensions
					pVPMgr->viewportPosition(ViewportUnderMouse).x(), pVPMgr->viewportPosition(ViewportUnderMouse).y(),
					pVPMgr->viewportSize(ViewportUnderMouse).x(), pVPMgr->viewportSize(ViewportUnderMouse).y());

				if (ActiveDatasets.size() == 1 || ActiveDisplayDataArrangement != DisplayDataArrangementMode::LAYERED) {
					for (auto Dataset : ActiveDatasets) {
						Matrix4f Model = pVPMgr->datasetModelMatrix(ViewportUnderMouse, Dataset);
						int PickedFace;
						Eigen::Vector3f PickPosBaryCoords;

						if (igl::embree::unproject_onto_mesh(
							CurrentCursorPosOGL, pDatasetGeometries.at(Dataset)->faces(), View * Model, Projection, VP,
							pDatasetGeometries.at(Dataset)->embreeIntersector(), PickedFace, PickPosBaryCoords)) {
							// calculate picked vertex, if any are within range of ray<->mesh intersection point
							Vector3i Face = pDatasetGeometries.at(Dataset)->face(PickedFace);
							Vector3f V0 = pDatasetGeometries.at(Dataset)->vertex(Face(0));
							Vector3f V1 = pDatasetGeometries.at(Dataset)->vertex(Face(1));
							Vector3f V2 = pDatasetGeometries.at(Dataset)->vertex(Face(2));
							Vector3f IntersectionPoint = (PickPosBaryCoords.x() * V0) + (PickPosBaryCoords.y() * V1) + (PickPosBaryCoords.z() * V2);
							
							Vector3f DistV0Intersect = IntersectionPoint - V0;
							Vector3f DistV1Intersect = IntersectionPoint - V1;
							Vector3f DistV2Intersect = IntersectionPoint - V2;

							float DistV0 = DistV0Intersect.squaredNorm();
							float DistV1 = DistV1Intersect.squaredNorm();
							float DistV2 = DistV2Intersect.squaredNorm();

							float MinDist = DistV0;		
							uint32_t VertexID = Face(0);

							if (DistV1 < MinDist) {
								MinDist = DistV1;
								VertexID = Face(1);
							}
							if (DistV2 < MinDist) {
								MinDist = DistV2;
								VertexID = Face(2);
							}
							
							if (MinDist < 0.06f) pGlobalAppState->newHoverPickResult(Dataset, VertexID, ViewportUnderMouse); // TODO adjust maximum distance after some testing
							else pGlobalAppState->newHoverPickResult(DatasetType::NONE, 0, 0);

							break;
						}
					}
				}
			}// end picking -> results are used below

			// process mouse button inputs
			bool MouseButtonPressed = false;

			// -> middle mouse button
			if (pRenderWin->mouse()->buttonState(Mouse::BTN_MIDDLE) == GLFW_PRESS) {

				pGlobalAppState->currentMMBCursorPos(CurrentCursorPosOGL);
				if (pGlobalAppState->oldMouseButtonState(Mouse::BTN_MIDDLE) == GLFW_RELEASE) pGlobalAppState->oldMMBCursorPos(pGlobalAppState->currentMMBCursorPos());

				if (pGlobalAppState->mouseButtonViewportFocus() < 0 && ViewportUnderMouse > -1) pGlobalAppState->mouseButtonViewportFocus(ViewportUnderMouse); // inputs are now applied to this viewport until mouse button is released

				if (pGlobalAppState->mouseButtonViewportFocus() > -1) {
					pVPMgr->viewportArcballRotate(pGlobalAppState->mouseButtonViewportFocus(), pGlobalAppState->oldMMBCursorPos(), pGlobalAppState->currentMMBCursorPos()); //TODO: arbcallRotateAllViewports if Alt / Shift key (or similar) is pressed
				}

				pGlobalAppState->oldMMBCursorPos(pGlobalAppState->currentMMBCursorPos());
				pGlobalAppState->oldMouseButtonState(Mouse::BTN_MIDDLE, GLFW_PRESS); // record this button press for next frame
				MouseButtonPressed = true;
			}
			else { // pRenderWin->mouse()->buttonState(Mouse::BTN_MIDDLE) == GLFW_RELEASE
				pGlobalAppState->currentMMBCursorPos(Vector2f::Zero());
				pGlobalAppState->oldMMBCursorPos(Vector2f::Zero());
				pGlobalAppState->mouseButtonViewportFocus(-1);
				pGlobalAppState->oldMouseButtonState(Mouse::BTN_MIDDLE, GLFW_RELEASE); // record unpressed button for next frame
			}// end if-else [pRenderWin->mouse()->buttonState(Mouse::BTN_MIDDLE) == GLFW_PRESS]

			// -> left mouse button
			if (pRenderWin->mouse()->buttonState(Mouse::BTN_LEFT) == GLFW_PRESS) {
				
				//TODO...

				pGlobalAppState->oldMouseButtonState(Mouse::BTN_LEFT, GLFW_PRESS); // record this button press for next frame
				MouseButtonPressed = true;
			}
			else { // pRenderWin->mouse()->buttonState(Mouse::BTN_LEFT) == GLFW_RELEASE
				
				//TODO...

				pGlobalAppState->oldMouseButtonState(Mouse::BTN_LEFT, GLFW_RELEASE); // record unpressed button for next frame
			}

			// -> right mouse button
			// temporary, remove later:
			if (!GuiIO.WantCaptureKeyboard && ViewportUnderMouse > -1)
				SceneUtilities::defaultCameraUpdate(pVPMgr->viewportCam(ViewportUnderMouse), pRenderWin->keyboard(), pRenderWin->mouse());

			//TODO:
			//if (pRenderWin->mouse()->buttonState(Mouse::BTN_RIGHT) == GLFW_PRESS) {
			//					
			//	//TODO...

			//	pGlobalAppState->oldMouseButtonState(Mouse::BTN_RIGHT, GLFW_PRESS); // record this button press for next frame
			//	MouseButtonPressed = true;
			//}
			//else { // pRenderWin->mouse()->buttonState(Mouse::BTN_RIGHT) == GLFW_RELEASE
			//	
			//	//TODO...

			//	pGlobalAppState->oldMouseButtonState(Mouse::BTN_RIGHT, GLFW_RELEASE); // record unpressed button for next frame
			//}

			// no mouse buttons were pressed, mouse cursor is only hovering over viewport
			if (!MouseButtonPressed && ViewportUnderMouse > -1) {
				auto CurPickRes = pGlobalAppState->currentHoverPickResult();

				if (CurPickRes.DT != DatasetType::NONE) {
					//pVPMgr->placeMarker(CurPickRes.ViewportID, CurPickRes.DT, CurPickRes.VertexID, MarkerMode::HOVER);
				}
				else {
					//pVPMgr->removeLastHoverMarker(CurPickRes.ViewportID);
				}
			}
		}
	}
}

#endif