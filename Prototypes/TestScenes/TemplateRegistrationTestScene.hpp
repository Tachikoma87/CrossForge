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

	void processInput(GLWindow& RenderWin, TempRegAppState& GlobalAppState, GUIManager& GUIMgr, ViewportManager& VPMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries);
	void raycastDatasets(
		Vector2f& CursorPosOGL, size_t VPIndex, ViewportManager& VPMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries,
		RayMeshIntersectionResult& RayMeshRes, RayPclIntersectionResult& RayPclRes);
	void pickPointOnMesh(Vector2f& CursorPosOGL, Vector4f& Viewport, Matrix4f& Model, Matrix4f& View, Matrix4f& Projection, DatasetGeometryData& Geometry, RayMeshIntersectionResult& RayMeshRes);
	void pickPointInPcl(Vector2f& CursorPosOGL, Vector4f& Viewport, Matrix4f& Model, Matrix4f& View, Matrix4f& Projection, DatasetGeometryData& Geometry, RayMeshIntersectionResult& RayPclRes);
	void selectDatasetComponentMesh(size_t VPIndex, ViewportManager& VPMgr, DatasetGeometryData& Geometry, const RayMeshIntersectionResult& RayMeshRes, PickingResult& PickRes);
	//void selectDatasetComponentPcl(...); //TODO

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
		ViewportManager VPMgr(4);
		VPMgr.initMarkerData("Assets/ExampleScenes/TempReg/UnitSphere.obj");
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
			processInput(RenderWin, GlobalAppState, GUIMgr, VPMgr, DatasetGeometries);

			// ready new Dear ImGui frame
			GUIMgr.buildNextImGuiFrame(GlobalAppState, VPMgr, DatasetGeometries);
						
			// render views
			bool ClearBuffer = true;
			for (size_t ID = 0; ID < VPMgr.activeViewportCount(); ++ID) {
				
				// update viewport position and dimension, camera for viewport, scene graph
				RDev.activeCamera(VPMgr.viewportCam(ID));
				VPMgr.updateViewport(ID, 60.0f / FPS);

				// render scene as usual
				RDev.viewport(GBufferVP);
				RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
				VPMgr.renderViewport(ID, RDev);
				RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				VPMgr.renderViewport(ID, RDev);

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

	void processInput(GLWindow& RenderWin, TempRegAppState& GlobalAppState, GUIManager& GUIMgr, ViewportManager& VPMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries) {

		// Undo last frame's state changes to correspondence point /feature point markers, default vertex hover markers, debug mesh markers
		// Has to happen even if ImGui wants to capture mouse / keyboard inputs!

		//TODO: something like this:
		/*if (GlobalAppState.oldPickResHover().ResultType == PickingResultType::CORRESPONDENCE_POINT) {
			auto& Dataset = DatasetGeometries.at(GlobalAppState.oldPickResHover().PickedDataset);
			uint32_t TemplateMarkerID = ... //TODO
			uint32_t TargetMarkerID = ... //TODO
			VPMgr.markerModeCorrMarkerPair(GlobalAppState.oldPickResHover().PickedDataset, TemplateMarkerID, TargetMarkerID, MarkerMode::IDLE);
		}*/
		
		//TODO: something like this:
		/*if (GlobalAppState.oldPickResHover().ResultType == PickingResultType::FEATURE_POINT) {
			auto& Dataset = DatasetGeometries.at(GlobalAppState.oldPickResHover().PickedDataset);
			uint32_t TemplateMarkerID = ... //TODO
			uint32_t TargetMarkerID = ... //TODO
			VPMgr.markerModeFeatMarkerPair(GlobalAppState.oldPickResHover().PickedDataset, TemplateMarkerID, TargetMarkerID, MarkerMode::IDLE);
		}*/

		for (size_t VP = 0; VP < VPMgr.activeViewportCount(); ++VP) {
			VPMgr.showDefaultVertexHoverMarkerOnDataset(VP, DatasetType::TEMPLATE, false);
			VPMgr.showDefaultVertexHoverMarkerOnDataset(VP, DatasetType::DTEMPLATE, false);
			VPMgr.showDefaultVertexHoverMarkerOnDataset(VP, DatasetType::TARGET, false);

			//DEBUG----------------------------------------------------------------------------------------------------
			VPMgr.DEBUG_showMeshMarkerOnDataset(VP, DatasetType::TEMPLATE, false);
			VPMgr.DEBUG_showMeshMarkerOnDataset(VP, DatasetType::DTEMPLATE, false);
			VPMgr.DEBUG_showMeshMarkerOnDataset(VP, DatasetType::TARGET, false);
			//DEBUG----------------------------------------------------------------------------------------------------
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// TODO: SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse()); //TODO disable WASD movement later, change to orbit camera (lookAt)
		// 
		// -> split between sections !GuiIO.WantCaptureKeyboard and !GuiIO.WantCaptureMouse!
		// -> execute updates per viewport!
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// IO states
		RayMeshIntersectionResult RayMeshIntersectResClick;
		RayPclIntersectionResult RayPclIntersectResClick;
		PickingResult PickResClick;
		RayMeshIntersectionResult RayMeshIntersectResHover;
		RayPclIntersectionResult RayPclIntersectResHover;
		PickingResult PickResHover;

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
		// NOTE: WantCaptureMouse == 0 if mouse buttons are pressed BEFORE cursor enters area of any ImGui widget! (click dragging while entering widgets will work properly)

		if (!GuiIO.WantCaptureMouse) { // mouse cursor is inside of viewport section area
			Vector2f CurrentCursorPosOGL = Vector2f(RenderWin.mouse()->position().x(), (float)RenderWin.height() - RenderWin.mouse()->position().y());

			// current position of mouse cursor in relation to active viewports; if ViewportUnderMouse == -1 the cursor does not hover over any active viewport
			int32_t ViewportUnderMouse = VPMgr.mouseInViewport(CurrentCursorPosOGL);

			// process mouse button inputs
			bool MouseHovering = true;

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// -> middle mouse button
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
			if (RenderWin.mouse()->buttonState(Mouse::BTN_MIDDLE) == GLFW_PRESS) {

				GlobalAppState.currentMMBCursorPos(CurrentCursorPosOGL);
				if (GlobalAppState.oldMouseButtonState(Mouse::BTN_MIDDLE) == GLFW_RELEASE) GlobalAppState.oldMMBCursorPos(GlobalAppState.currentMMBCursorPos());

				if (GlobalAppState.mouseButtonViewportFocus() < 0 && ViewportUnderMouse > -1) GlobalAppState.mouseButtonViewportFocus(ViewportUnderMouse); // inputs are now applied to this viewport until mouse button is released

				if (GlobalAppState.mouseButtonViewportFocus() > -1)
					VPMgr.viewportArcballRotate(GlobalAppState.mouseButtonViewportFocus(), GlobalAppState.oldMMBCursorPos(), GlobalAppState.currentMMBCursorPos()); //TODO: arbcallRotateAllViewports if Alt / Shift key (or similar) is pressed

				GlobalAppState.oldMMBCursorPos(GlobalAppState.currentMMBCursorPos());
				GlobalAppState.oldMouseButtonState(Mouse::BTN_MIDDLE, GLFW_PRESS); // record this button press for next frame
				MouseHovering = false;
			}
			else { // pRenderWin->mouse()->buttonState(Mouse::BTN_MIDDLE) == GLFW_RELEASE //TODO: will these states update correctly if MMB is released while mouse cursor is over GUI widgets?
				GlobalAppState.currentMMBCursorPos(Vector2f::Zero());
				GlobalAppState.oldMMBCursorPos(Vector2f::Zero());
				GlobalAppState.mouseButtonViewportFocus(-1);
				GlobalAppState.oldMouseButtonState(Mouse::BTN_MIDDLE, GLFW_RELEASE); // record unpressed button for next frame
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// -> left mouse button
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
			if (RenderWin.mouse()->buttonState(Mouse::BTN_LEFT) == GLFW_PRESS) {
				
				//TODO...
				//TODO: selecting dataset components with LMB -> left clicks on a dataset component should always change the last selected component to this newly clicked one!
				//TODO: selecting dataset components with LMB -> left clicks outside of any active dataset aswell as outside of any GUI widgets should cause a deselection of the last selected component!

				GlobalAppState.oldMouseButtonState(Mouse::BTN_LEFT, GLFW_PRESS); // record this button press for next frame
				MouseHovering = false;
			}
			else { // pRenderWin->mouse()->buttonState(Mouse::BTN_LEFT) == GLFW_RELEASE
				
				//TODO...

				GlobalAppState.oldMouseButtonState(Mouse::BTN_LEFT, GLFW_RELEASE); // record unpressed button for next frame
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// -> right mouse button
			// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
			// temporary, remove later:
			if (!GuiIO.WantCaptureKeyboard && ViewportUnderMouse > -1)
				SceneUtilities::defaultCameraUpdate(VPMgr.viewportCam(ViewportUnderMouse), RenderWin.keyboard(), RenderWin.mouse());

			//TODO:
			//if (pRenderWin->mouse()->buttonState(Mouse::BTN_RIGHT) == GLFW_PRESS) {
			//					
			//	//TODO...

			//	pGlobalAppState->oldMouseButtonState(Mouse::BTN_RIGHT, GLFW_PRESS); // record this button press for next frame
			//	MouseHovering = false;
			//}
			//else { // pRenderWin->mouse()->buttonState(Mouse::BTN_RIGHT) == GLFW_RELEASE
			//	
			//	//TODO...

			//	pGlobalAppState->oldMouseButtonState(Mouse::BTN_RIGHT, GLFW_RELEASE); // record unpressed button for next frame
			//}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// no mouse buttons were pressed, mouse cursor is only hovering over viewport
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			if (MouseHovering && ViewportUnderMouse > -1) {
								
				raycastDatasets(CurrentCursorPosOGL, ViewportUnderMouse, VPMgr, DatasetGeometries, RayMeshIntersectResHover, RayPclIntersectResHover);
				
				if (RayMeshIntersectResHover.IntersectedDataset != DatasetType::NONE) {
					// find closest component to intersection point: mesh vertex, correspondence point (if displayed), etc...
					//TODO: consider active interaction mode of application (editing feature pairs, ...)
					selectDatasetComponentMesh(ViewportUnderMouse, VPMgr, DatasetGeometries.at(RayMeshIntersectResHover.IntersectedDataset), RayMeshIntersectResHover, PickResHover); //TODO

					if (PickResHover.ResultType == PickingResultType::MESH_VERTEX) {
						//TODO: consider pre-existing markers at selected component -> DefaultVertexSelectMarker

						Vector3f Vertex = DatasetGeometries.at(RayMeshIntersectResHover.IntersectedDataset).vertex(PickResHover.PickedVertex);
						VPMgr.placeDefaultVertexHoverMarker(ViewportUnderMouse, PickResHover.PickedDataset, Vertex);
						VPMgr.showDefaultVertexHoverMarkerOnDataset(ViewportUnderMouse, PickResHover.PickedDataset, true);
					}

					if (PickResHover.ResultType == PickingResultType::CORRESPONDENCE_POINT) {
						//TODO: consider pre-existing markers at selected component -> DefaultVertexSelectMarker, correspondence point / feature point markers
						//...
												
					}

					if (PickResHover.ResultType == PickingResultType::FEATURE_POINT) {
						//TODO: consider pre-existing markers at selected component -> DefaultVertexSelectMarker, correspondence point / feature point markers
						//...
					}
					
					//DEBUG----------------------------------------------------------------------------------------------------
					VPMgr.DEBUG_placeMeshMarker(ViewportUnderMouse, RayMeshIntersectResHover.IntersectedDataset, RayMeshIntersectResHover.IntersectionPos);
					VPMgr.DEBUG_showMeshMarkerOnDataset(ViewportUnderMouse, RayMeshIntersectResHover.IntersectedDataset, true);
					//DEBUG----------------------------------------------------------------------------------------------------
				}

				//TODO:
				//if (PclPickRes.IntersectedDataset != DatasetType::NONE) {
				//	//...
				//}
			}
		}
		GlobalAppState.oldPickResClick(GlobalAppState.currentPickResClick());
		GlobalAppState.oldPickResHover(GlobalAppState.currentPickResHover());
		GlobalAppState.currentPickResClick(PickResClick);
		GlobalAppState.currentPickResHover(PickResHover);
	}//processInput

	// raycast against datasets (for picking / highlighting of vertices) if mouse cursor hovers over a viewport
	// only do raycast when: 
	// a) exactly one dataset is active in given viewport or
	// b) mutliple datasets are displayed side by side (NOT layered over each other)
	void raycastDatasets(
		Vector2f& CursorPosOGL, size_t VPIndex, ViewportManager& VPMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries,
		RayMeshIntersectionResult& RayMeshRes, RayPclIntersectionResult& RayPclRes) {
		if (VPIndex > VPMgr.activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto ActiveDatasets = VPMgr.activeDatasetTypes(VPIndex);
		auto ActiveDisplayDataArrangement = VPMgr.activeDatasetDisplayDataArrangement(VPIndex);
		if (ActiveDatasets.size() == 1 || ActiveDisplayDataArrangement != DisplayDataArrangementMode::LAYERED) {
			for (auto Dataset : ActiveDatasets) {

				auto& Geometry = DatasetGeometries.at(Dataset);
				Vector4f Viewport = Vector4f(
					VPMgr.viewportPosition(VPIndex).x(), VPMgr.viewportPosition(VPIndex).y(),
					VPMgr.viewportSize(VPIndex).x(), VPMgr.viewportSize(VPIndex).y());
				Matrix4f View = VPMgr.viewportViewMatrix(VPIndex);
				Matrix4f Projection = VPMgr.viewportProjectionMatrix(VPIndex);
				Matrix4f Model = VPMgr.datasetModelMatrix(VPIndex, Dataset);

				// pick point on mesh
				if (Geometry.geometryType() == DatasetGeometryType::MESH) {
					pickPointOnMesh(CursorPosOGL, Viewport, Model, View, Projection, Geometry, RayMeshRes);

					if (RayMeshRes.IntersectedFace >= 0) {
						RayMeshRes.IntersectedDataset = Dataset;
						break;
					}
				}
				else { // pick feature in point cloud
					//TODO
				}
			}
		}
	}//raycastDatasets

	void pickPointOnMesh(
		Vector2f& CursorPosOGL, Vector4f& Viewport, Matrix4f& Model, Matrix4f& View, Matrix4f& Projection, DatasetGeometryData& Geometry, RayMeshIntersectionResult& RayMeshRes) {
		if (igl::embree::unproject_onto_mesh(
			CursorPosOGL, Geometry.faces(), View * Model, Projection, Viewport,
			Geometry.embreeIntersector(), RayMeshRes.IntersectedFace, RayMeshRes.BarycentricCoords)) {
			// calculate picked vertex, if any are within range of ray<->mesh intersection point
			Vector3i Face = Geometry.face(RayMeshRes.IntersectedFace);
			Vector3f V0 = Geometry.vertex(Face(0));
			Vector3f V1 = Geometry.vertex(Face(1));
			Vector3f V2 = Geometry.vertex(Face(2));
			RayMeshRes.IntersectionPos = (RayMeshRes.BarycentricCoords.x() * V0) + (RayMeshRes.BarycentricCoords.y() * V1) + (RayMeshRes.BarycentricCoords.z() * V2);
		}
	}//pickPointOnMesh

	void selectDatasetComponentMesh(size_t VPIndex, ViewportManager& VPMgr, DatasetGeometryData& Geometry, const RayMeshIntersectionResult& RayMeshRes, PickingResult& PickRes) { //TODO
		if (VPIndex > VPMgr.activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		
		// decide which component of the dataset to select: vertices, correspondence/feature points, etc...
		
		bool FeatInRange = false;
		bool CorrInRange = false;
		bool VertInRange = false;
		std::vector<uint32_t> FeatPointsInRange;
		std::vector<uint32_t> CorrPointsInRange;
		float DistToClosestVert = FLT_MAX;
		uint32_t ClosestVert;

		//TODO: check for feature points in range of cursor 	
		// -> check if any feature point pairs are linked to the picked mesh face; get all feature points in range of cursor	
		//...
				
		//TODO: check for correspondence points in range of cursor
		// -> check if any correspondence point pairs are linked to the picked mesh face;  get all correspondence points in range of cursor
		//...

		// check if a vertex is in range of cursor		
		Vector3i Face = Geometry.face(RayMeshRes.IntersectedFace);
		Vector3f V0 = Geometry.vertex(Face(0));
		Vector3f V1 = Geometry.vertex(Face(1));
		Vector3f V2 = Geometry.vertex(Face(2));

		float DistSqV0ToIntersect = (V0 - RayMeshRes.IntersectionPos).squaredNorm();
		float DistSqV1ToIntersect = (V1 - RayMeshRes.IntersectionPos).squaredNorm();
		float DistSqV2ToIntersect = (V2 - RayMeshRes.IntersectionPos).squaredNorm();

		DistToClosestVert = DistSqV0ToIntersect;
		ClosestVert = 0;
		
		if (DistSqV1ToIntersect < DistToClosestVert) {
			DistToClosestVert = DistSqV1ToIntersect;
			ClosestVert = 1;
		}

		if (DistSqV2ToIntersect < DistToClosestVert) {
			DistToClosestVert = DistSqV2ToIntersect;
			ClosestVert = 2;
		}

		if (DistToClosestVert < 0.03f) VertInRange = true;

		// choose mesh component closest to cursor - priority: feature points = correspondence points > vertices
		
		if (FeatInRange) {
			PickRes.ViewportID = VPIndex;
			PickRes.PickedDataset = RayMeshRes.IntersectedDataset;
			PickRes.ResultType = PickingResultType::FEATURE_POINT;
			//MeshPickRes.PickedFeaturePoint = ...
		}

		if (CorrInRange) {
			PickRes.ViewportID = VPIndex;
			PickRes.PickedDataset = RayMeshRes.IntersectedDataset;
			PickRes.ResultType = PickingResultType::CORRESPONDENCE_POINT;
			//MeshPickRes.PickedCorrPoint = ...
		}

		if (VertInRange && !FeatInRange && !CorrInRange) {
			PickRes.ViewportID = VPIndex;
			PickRes.PickedDataset = RayMeshRes.IntersectedDataset;
			PickRes.ResultType = PickingResultType::MESH_VERTEX;
			PickRes.PickedVertex = Face(ClosestVert);
		}
	}//selectDatasetComponentMesh
}

#endif