#include "ViewportInputManager.h"

#include "../../CForge/Input/Keyboard.h"
#include "../../CForge/Input/Mouse.h"
#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../Examples/SceneUtilities.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <igl/embree/unproject_onto_mesh.h>

namespace TempReg {

	ViewportInputManager::ViewportInputManager() :
		m_ManualCorrTemplateReady(false), m_ManualCorrTargetReady(false) {

	}//Constructor

	ViewportInputManager::~ViewportInputManager() {

	}//Destructor

	void ViewportInputManager::processInput(
		CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, GUIManager& GUIMgr, ViewportRenderManager& VPRMgr,
		std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage) {

		// TEMPORARY CODE FOR TESTING ====================================================================================================================
		// force display of correspondence markers in all viewports
		for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) {
			VPRMgr.showCorrMarkersOnDataset(VP, DatasetType::TEMPLATE, true);
			VPRMgr.showCorrMarkersOnDataset(VP, DatasetType::DTEMPLATE, true);
			VPRMgr.showCorrMarkersOnDataset(VP, DatasetType::TARGET, true);
		}
		// END TEMPORARY CODE FOR TESTING ================================================================================================================

		const ImGuiIO& GuiIO = ImGui::GetIO();
		const auto LMBState = RenderWin.mouse()->buttonState(CForge::Mouse::BTN_LEFT);
		const auto MMBState = RenderWin.mouse()->buttonState(CForge::Mouse::BTN_MIDDLE);
		const auto RMBState = RenderWin.mouse()->buttonState(CForge::Mouse::BTN_RIGHT);
		
		// only forward keyboard / mouse input data to viewport controls if not captured by gui
		// WantCaptureKeyboard/WantCaptureMouse flags updated by ImGui::NewFrame(): calling processInput(...) before calling NewFrame() to poll flags from previous frame allows for more reliable input processing
		// (as per: https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-tell-whether-to-dispatch-mousekeyboard-to-dear-imgui-or-my-application)
		// NOTE: WantCaptureMouse == 0 if mouse buttons are pressed BEFORE cursor enters area of any ImGui widget! (click dragging while entering widgets will work properly)

		if (!GuiIO.WantCaptureKeyboard) {
			processKeyboard(RenderWin, GlobalAppState, VPRMgr, CorrStorage); //TODO
		}

		if (!GuiIO.WantCaptureMouse) { // mouse cursor is inside of viewport section area
			Vector2f CurrentCursorPosOGL = Vector2f(RenderWin.mouse()->position().x(), (float)RenderWin.height() - RenderWin.mouse()->position().y());
			int32_t ViewportUnderMouse = VPRMgr.mouseInViewport(CurrentCursorPosOGL); // if ViewportUnderMouse == -1 the cursor does not hover over any active viewport //TODO: store ViewportUnderMouse in GlobalAppState!

			if (ViewportUnderMouse != -1)
				raycastDatasets(GlobalAppState, VPRMgr, DatasetGeometries, CurrentCursorPosOGL, ViewportUnderMouse);

			// process mouse inputs
			
			// -> process middle mouse button
			if (MMBState == GLFW_PRESS) {
				if (GlobalAppState.mouseButtonViewportFocus() < 0 && ViewportUnderMouse != -1) GlobalAppState.mouseButtonViewportFocus(ViewportUnderMouse); // inputs are now applied to this viewport until mouse button is released
				processMMBDown(RenderWin, GlobalAppState, VPRMgr, CurrentCursorPosOGL, ViewportUnderMouse, GuiIO.WantCaptureKeyboard);
			}
			else {
				processMMBUp(GlobalAppState);
			}
			// end processing of middle mouse button

			if (ViewportUnderMouse != -1) {

				// -> process left mouse button
				if (LMBState == GLFW_RELEASE && GlobalAppState.prevMouseButtonState(CForge::Mouse::BTN_LEFT) == GLFW_PRESS) { // execute processing on button release to make sure processing is not repeated next frame
					processLMBDown(RenderWin, GlobalAppState, VPRMgr, DatasetGeometries, CorrStorage, CurrentCursorPosOGL, ViewportUnderMouse, GuiIO.WantCaptureKeyboard);
				}
				// end processing of left mouse button

				// -> process right mouse button
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// 
				// TODO: SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse()); //TODO disable WASD movement later, change to orbit camera (lookAt)
				// 
				// -> split between sections !GuiIO.WantCaptureKeyboard and !GuiIO.WantCaptureMouse!
				// -> execute updates per viewport!
				//
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// temporary, remove later:
				if (!GuiIO.WantCaptureKeyboard)
					CForge::SceneUtilities::defaultCameraUpdate(VPRMgr.viewportCam(ViewportUnderMouse), RenderWin.keyboard(), RenderWin.mouse());

				if (RMBState == GLFW_PRESS) {
					processRMBDown(RenderWin, GlobalAppState, VPRMgr, DatasetGeometries, CorrStorage, CurrentCursorPosOGL, ViewportUnderMouse, GuiIO.WantCaptureKeyboard);	
				}
				// end processing of right mouse button


				// -> process hovering mouse cursor
				processMouseHover(RenderWin, GlobalAppState, VPRMgr, DatasetGeometries, CorrStorage, CurrentCursorPosOGL, ViewportUnderMouse, GuiIO.WantCaptureKeyboard);
			}
		}
		
		// store this frame's mouse button states for next frame
		GlobalAppState.prevMouseButtonState(CForge::Mouse::BTN_LEFT, LMBState);
		GlobalAppState.prevMouseButtonState(CForge::Mouse::BTN_MIDDLE, MMBState);
		GlobalAppState.prevMouseButtonState(CForge::Mouse::BTN_RIGHT, RMBState);
	}//processInput

	void ViewportInputManager::processKeyboard(CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, CorrespondenceStorage& CorrStorage) {
		if (RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_DELETE) && GlobalAppState.prevSelectRes().ResultType == PickingResultType::CORRESPONDENCE_PAIR) {
			CorrespondenceStorage::Correspondence Corr;
			CorrStorage.correspondenceAtTemplateVertex(Corr, GlobalAppState.prevSelectRes().TemplatePointID);
			std::vector<size_t> RemainingTargetPartners;
			CorrStorage.removeCorrespondence(GlobalAppState.prevSelectRes().TemplatePointID, &RemainingTargetPartners);
			VPRMgr.removeCorrMarkerTemplate(GlobalAppState.prevSelectRes().TemplatePointID);

			bool KeepTargetMarker = false;
			for (auto P : RemainingTargetPartners) {
				CorrespondenceStorage::Correspondence C;
				CorrStorage.correspondenceAtTemplateVertex(C, P);
				if (C.Type == CorrespondenceType::CORRESPONDENCE) {
					KeepTargetMarker = true;
					break;
				}
			}

			if (!KeepTargetMarker) VPRMgr.removeCorrMarkerTarget(GlobalAppState.prevSelectRes().TargetPointID);
			else VPRMgr.markerModeCorrMarkerTarget(GlobalAppState.prevSelectRes().TargetPointID, MarkerMode::IDLE);

			GlobalAppState.prevSelectRes(TempRegAppState::PickingResult());
		}

		if (RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_BACKSPACE)) {
			CorrStorage.clearCorrespondences();
			VPRMgr.clearCorrMarkers();

			GlobalAppState.prevSelectRes(TempRegAppState::PickingResult());
		}
	}//processKeyboard

	void ViewportInputManager::processLMBDown(
		CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage,
		Vector2f CurrentCursorPosOGL, int32_t ViewportUnderMouse, bool CaptureKeyboard) {

		TempRegAppState::PickingResult CurrSelectRes;
		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();

		// -> reset markers of correspondence pairs selected while pressing LEFT_ALT
		//const auto& PrevSelection = GlobalAppState.prevSelectRes();
		//clearPreviousSelectionHighlighting(PrevSelection, VPRMgr);

		// -> special case on LMB + LEFT ALT: only allow selection of complete correspondence pairs in this frame
		if (!CaptureKeyboard && RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_LEFT_ALT)) {
			// ONLY select existing correspondence at template vertex
			//TODO: create GUI widgets to explicitly choose between creation and deletion of correspondences, so that the selection of existing correspondences can happen 
			//      through all datasets (TEMPLATE / DTEMPLATE, TARGET) without interferring with manual correspondence creation or requiring extra keys
			lmbCorrPairSelection(CurrSelectRes, GlobalAppState, VPRMgr, ViewportUnderMouse, DatasetGeometries, CorrStorage, RayIntersectRes.Dataset);
			GlobalAppState.prevSelectRes(CurrSelectRes);
			return;
		}

		// process left clicks on TEMPLATE/DTEMPLATE dataset
		if (RayIntersectRes.Dataset == DatasetType::TEMPLATE || RayIntersectRes.Dataset == DatasetType::DTEMPLATE)
			lmbTemplateVertexCorrPointSelection(CurrSelectRes, GlobalAppState, VPRMgr, ViewportUnderMouse, DatasetGeometries, CorrStorage);

		// process left clicks on TARGET dataset
		if (RayIntersectRes.Dataset == DatasetType::TARGET && RayIntersectRes.GeometryType == DatasetGeometryType::MESH) {
			if (!CaptureKeyboard && RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_LEFT_CONTROL)) {
				// correspondence pairs of type vertex -> arbitrary surface points
				lmbTargetSurfPointCorrPointSelection(CurrSelectRes, GlobalAppState, VPRMgr, ViewportUnderMouse, DatasetGeometries, CorrStorage);
			}
			else { // general vertex selection for RayIntersectRes.GeometryType == DatasetGeometryType::MESH and RayIntersectRes.GeometryType == DatasetGeometryType::POINTCLOUD
				lmbTargetVertexCorrPointSelection(CurrSelectRes, GlobalAppState, VPRMgr, ViewportUnderMouse, DatasetGeometries, CorrStorage);
			}
		}

		// process left clicks into empty 3D space
		if (RayIntersectRes.Dataset == DatasetType::NONE)
			lmbEmptySelection(CurrSelectRes, GlobalAppState, VPRMgr);

		if (m_ManualCorrTemplateReady && m_ManualCorrTargetReady)
			processManualCorrespondenceCreation(CurrSelectRes, GlobalAppState, VPRMgr, DatasetGeometries, CorrStorage);

		// remember what happened this frame
		GlobalAppState.prevSelectRes(CurrSelectRes);
	}//processLMBDown

	void ViewportInputManager::clearPreviousSelectionHighlighting(const TempRegAppState::PickingResult& PrevSelection, ViewportRenderManager& VPRMgr) {
		if (PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_PAIR)
			VPRMgr.markerModeCorrMarkerPair(PrevSelection.TemplatePointID, PrevSelection.TargetPointID, MarkerMode::IDLE);

		if (PrevSelection.Dataset == DatasetType::TARGET &&
			(PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_POINT_VERTEX || PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_POINT_SURFACE))
			VPRMgr.markerModeCorrMarkerTarget(PrevSelection.TargetPointID, MarkerMode::IDLE);
	}//clearPreviousSelectionHighlighting

	void ViewportInputManager::lmbCorrPairSelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
		std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage, DatasetType DT) {

		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();
		const auto& PrevSelection = GlobalAppState.prevSelectRes();

		if (PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_PAIR)
			VPRMgr.markerModeCorrMarkerPair(PrevSelection.TemplatePointID, PrevSelection.TargetPointID, MarkerMode::IDLE);

		// clear manual configuration for next correspondence
		if (m_ManualCorrTemplateReady) {
			for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) {
				VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TEMPLATE, false);
				VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::DTEMPLATE, false);
			}

			m_ManualCorrData.TemplatePointUID = 0;
			m_ManualCorrData.VertexVertexPair = false;
			m_ManualCorrData.Type = CorrespondenceType::NONE;

			m_ManualCorrTemplateReady = false;
		}

		if (m_ManualCorrTargetReady) {
			if (PrevSelection.ResultType == PickingResultType::MESH_VERTEX || PrevSelection.ResultType == PickingResultType::MESH_SURFACE_POINT) {
				for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) 
					VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TARGET, false); 
			}

			if (PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_POINT_VERTEX || PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_POINT_SURFACE) {
				VPRMgr.markerModeCorrMarkerTarget(PrevSelection.TargetPointID, MarkerMode::IDLE);
			}
			
			m_ManualCorrData.TargetPointUID = 0;
			m_ManualCorrData.TargetFaceID = 0;
			m_ManualCorrData.TargetPointPos = Vector3f::Zero();
			m_ManualCorrData.TargetPointBaryCoords = Vector3f::Zero();
			m_ManualCorrData.VertexVertexPair = false;
			m_ManualCorrData.Type = CorrespondenceType::NONE;

			m_ManualCorrTargetReady = false;
		}
		
		if (DT == DatasetType::TEMPLATE || DT == DatasetType::DTEMPLATE) {

			int64_t PickedVertex = pickVertex(RayIntersectRes, DatasetGeometries.at(RayIntersectRes.Dataset), 0.015f);
			if (PickedVertex != -1) {

				CorrespondenceStorage::Correspondence Corr;
				if (CorrStorage.correspondenceAtTemplateVertex(Corr, PickedVertex)) {

					// for testing purposes it is simply assumed that the correspondence at this template vertex is not a feature point pair!
					CurrSelectRes.ViewportID = ViewportUnderMouse;
					CurrSelectRes.Dataset = RayIntersectRes.Dataset;
					CurrSelectRes.ResultType = PickingResultType::CORRESPONDENCE_PAIR;
					CurrSelectRes.TemplatePointID = Corr.TemplatePointUID;
					CurrSelectRes.TargetPointID = Corr.TargetPointUID;
					CurrSelectRes.Face = RayIntersectRes.Face;

					VPRMgr.markerModeCorrMarkerPair(Corr.TemplatePointUID, Corr.TargetPointUID, MarkerMode::SELECTED);

					//TempRegAppState::PickingResult CurrHoverRes;
					//CurrHoverRes.ViewportID = ViewportUnderMouse;
					//CurrHoverRes.Dataset = DatasetType::NONE;
					//CurrHoverRes.ResultType = PickingResultType::NONE;
					//CurrHoverRes.Face = 0;
					//CurrHoverRes.TemplatePointID = 0;
					//CurrHoverRes.TargetPointID = 0;
					//CurrHoverRes.SurfacePoint = Vector3f::Zero();
					//GlobalAppState.prevHoverRes(CurrHoverRes);
				}
			}
		}
		else {
			// -> hide default selection markers
			for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) {
				VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TEMPLATE, false);
				VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::DTEMPLATE, false);
				VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TARGET, false);
			}

			// clear selection data of this frame
			CurrSelectRes.ViewportID = ViewportUnderMouse;
			CurrSelectRes.Dataset = DatasetType::NONE;
			CurrSelectRes.ResultType = PickingResultType::NONE;
			CurrSelectRes.Face = 0;
			CurrSelectRes.TemplatePointID = 0;
			CurrSelectRes.TargetPointID = 0;
			CurrSelectRes.SurfacePoint = Vector3f::Zero();
		}
	}//lmbCorrPairSelection

	void ViewportInputManager::lmbTemplateVertexCorrPointSelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
		std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage) {

		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();

		int64_t PickedVertex = pickVertex(RayIntersectRes, DatasetGeometries.at(RayIntersectRes.Dataset), 0.015f);
		if (PickedVertex == -1) return; // no vertex in reach, nothing more to do
		
		if (CorrStorage.templateVertexAvailable(PickedVertex)) {
			// template vertex is not part of any correspondences

			CurrSelectRes.ViewportID = ViewportUnderMouse;
			CurrSelectRes.Dataset = RayIntersectRes.Dataset;
			CurrSelectRes.TemplatePointID = PickedVertex;
			CurrSelectRes.Face = RayIntersectRes.Face;
			CurrSelectRes.ResultType = PickingResultType::MESH_VERTEX;

			m_ManualCorrData.TemplatePointUID = PickedVertex;
			m_ManualCorrData.Type = CorrespondenceType::CORRESPONDENCE;

			m_ManualCorrTemplateReady = true;

			VPRMgr.placeDefaultSelectMarker(DatasetType::TEMPLATE, DatasetGeometries.at(DatasetType::TEMPLATE).vertex(PickedVertex), MarkerColor::CORRESPONDENCE_SELECTED);
			VPRMgr.placeDefaultSelectMarker(DatasetType::DTEMPLATE, DatasetGeometries.at(DatasetType::DTEMPLATE).vertex(PickedVertex), MarkerColor::CORRESPONDENCE_SELECTED);

			for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) {
				VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TEMPLATE, true); //TODO: should be controlled by GUI instead (viewport input processing should only *place* markers)
				VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::DTEMPLATE, true); //TODO: should be controlled by GUI instead (viewport input processing should only *place* markers)
			}
		}
		else {
			//TODO: GUI popup "template vertex already in use!" if target data for manual correspondence is set
		}
	}//lmbTemplateVertexCorrPointSelection

	void ViewportInputManager::lmbTargetSurfPointCorrPointSelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
		std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage) {

		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();

		CurrSelectRes.ViewportID = ViewportUnderMouse;
		CurrSelectRes.Dataset = DatasetType::TARGET;
		CurrSelectRes.Face = RayIntersectRes.Face;
		bool CorrPointIsVertex = true;

		int64_t PickedVertex = pickVertex(RayIntersectRes, DatasetGeometries.at(DatasetType::TARGET), (float)1e-4);
		if (PickedVertex != -1) {
			// picked surface point is a target vertex

			CurrSelectRes.TargetPointID = PickedVertex;

			// check if target vertex already has correspondence data
			if (CorrStorage.targetCorrespondencePointExists(PickedVertex)) {
				CurrSelectRes.ResultType = PickingResultType::CORRESPONDENCE_POINT_VERTEX;

				// set correspondence marker to SELECTED mode instead of placing down the default select marker
				VPRMgr.markerModeCorrMarkerTarget(CurrSelectRes.TargetPointID, MarkerMode::SELECTED);
			}
			else {
				CurrSelectRes.ResultType = PickingResultType::MESH_VERTEX;

				// mimic a correspondence marker
				VPRMgr.placeDefaultSelectMarker(DatasetType::TARGET, DatasetGeometries.at(DatasetType::TARGET).vertex(PickedVertex), MarkerColor::CORRESPONDENCE_SELECTED);
				for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP)
					VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TARGET, true); //TODO: should be controlled by GUI instead (viewport input processing should only *place* markers)
			}
		}
		else {
			// surface point picked by raycast is not a target vertex
			CorrPointIsVertex = false;

			// check if a target surface point with correspondence data exists instead; if so, it must have a marker -> set marker to SELECTED mode instead of placing down the default select marker
			size_t TargetPointUID;
			if (CorrStorage.targetCorrespondencePointExists(RayIntersectRes.Face, RayIntersectRes.IntersectionPos, TargetPointUID)) {

				CurrSelectRes.ResultType = PickingResultType::CORRESPONDENCE_POINT_SURFACE;
				CurrSelectRes.TargetPointID = TargetPointUID;
				CurrSelectRes.SurfacePoint = CorrStorage.targetSurfacePointPosition(TargetPointUID);

				VPRMgr.markerModeCorrMarkerTarget(TargetPointUID, MarkerMode::SELECTED);
			}
			else { // target surface point is new

				CurrSelectRes.ResultType = PickingResultType::MESH_SURFACE_POINT;
				CurrSelectRes.TargetPointID = 0;
				CurrSelectRes.SurfacePoint = RayIntersectRes.IntersectionPos;

				VPRMgr.placeDefaultSelectMarker(DatasetType::TARGET, RayIntersectRes.IntersectionPos, MarkerColor::CORRESPONDENCE_SELECTED);
				for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP)
					VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TARGET, true); //TODO: should be controlled by GUI instead (viewport input processing should only *place* markers)
			}
		}

		m_ManualCorrData.TargetPointUID = CurrSelectRes.TargetPointID;
		m_ManualCorrData.TargetFaceID = CurrSelectRes.Face;
		m_ManualCorrData.TargetPointPos = (CorrPointIsVertex) ? DatasetGeometries.at(DatasetType::TARGET).vertex(CurrSelectRes.TargetPointID) : CurrSelectRes.SurfacePoint;
		m_ManualCorrData.TargetPointBaryCoords = (CorrPointIsVertex) ? Vector3f::Zero() : RayIntersectRes.BarycentricCoords;
		m_ManualCorrData.VertexVertexPair = CorrPointIsVertex;
		m_ManualCorrData.Type = CorrespondenceType::CORRESPONDENCE;

		m_ManualCorrTargetReady = true;
	}//lmbTargetSurfPointCorrPointSelection

	void ViewportInputManager::lmbTargetVertexCorrPointSelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
		std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage) {

		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();

		int64_t PickedVertex = pickVertex(RayIntersectRes, DatasetGeometries.at(DatasetType::TARGET), 0.015f);
		if (PickedVertex == -1) return; // no vertex in reach, nothing more to do

		CurrSelectRes.ViewportID = ViewportUnderMouse;
		CurrSelectRes.Dataset = DatasetType::TARGET;
		CurrSelectRes.TargetPointID = PickedVertex;
		CurrSelectRes.Face = RayIntersectRes.Face;

		m_ManualCorrData.TargetPointUID = CurrSelectRes.TargetPointID;
		m_ManualCorrData.TargetFaceID = CurrSelectRes.Face;
		m_ManualCorrData.TargetPointPos = DatasetGeometries.at(DatasetType::TARGET).vertex(CurrSelectRes.TargetPointID);
		m_ManualCorrData.TargetPointBaryCoords = Vector3f::Zero();
		m_ManualCorrData.VertexVertexPair = true;
		m_ManualCorrData.Type = CorrespondenceType::CORRESPONDENCE;

		m_ManualCorrTargetReady = true;

		// set marker
		if (CorrStorage.targetCorrespondencePointExists(PickedVertex)) {
			CurrSelectRes.ResultType = PickingResultType::CORRESPONDENCE_POINT_VERTEX;

			// set correspondence marker to SELECTED mode instead of placing down the default select marker
			VPRMgr.markerModeCorrMarkerTarget(CurrSelectRes.TargetPointID, MarkerMode::SELECTED);
		}
		else {
			// target vertex is new				
			CurrSelectRes.ResultType = PickingResultType::MESH_VERTEX;

			VPRMgr.placeDefaultSelectMarker(DatasetType::TARGET, DatasetGeometries.at(DatasetType::TARGET).vertex(PickedVertex), MarkerColor::CORRESPONDENCE_SELECTED);
			for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP)
				VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TARGET, true); //TODO: should be controlled by GUI instead (viewport input processing should only *place* markers)
		}
	}//lmbTargetVertexCorrPointSelection

	void ViewportInputManager::lmbEmptySelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr) {

		// hide default selection markers
		for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) {
			VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TEMPLATE, false);
			VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::DTEMPLATE, false);
			VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TARGET, false);
		}

		// reset correspondence markers for selected correspondence pairs
		if (GlobalAppState.prevSelectRes().ResultType == PickingResultType::CORRESPONDENCE_PAIR)
			VPRMgr.markerModeCorrMarkerPair(GlobalAppState.prevSelectRes().TemplatePointID, GlobalAppState.prevSelectRes().TargetPointID, MarkerMode::IDLE);

		// reset markers for selected target correspondence points (vertex or surface point)
		if (GlobalAppState.prevSelectRes().Dataset == DatasetType::TARGET && 
			(GlobalAppState.prevSelectRes().ResultType == PickingResultType::CORRESPONDENCE_POINT_VERTEX || 
				GlobalAppState.prevSelectRes().ResultType == PickingResultType::CORRESPONDENCE_POINT_SURFACE)) {
			VPRMgr.markerModeCorrMarkerTarget(GlobalAppState.prevSelectRes().TargetPointID, MarkerMode::IDLE);
		}

		// clear manual configuration for next correspondence
		m_ManualCorrData.TemplatePointUID = 0;
		m_ManualCorrData.TargetPointUID = 0;
		m_ManualCorrData.TargetFaceID = 0;
		m_ManualCorrData.TargetPointPos = Vector3f::Zero();
		m_ManualCorrData.TargetPointBaryCoords = Vector3f::Zero();
		m_ManualCorrData.VertexVertexPair = false;
		m_ManualCorrData.Type = CorrespondenceType::NONE;

		m_ManualCorrTemplateReady = false;
		m_ManualCorrTargetReady = false;

		// empty selection data of this frame
		CurrSelectRes.ViewportID = 0;
		CurrSelectRes.Dataset = DatasetType::NONE;
		CurrSelectRes.ResultType = PickingResultType::NONE;
		CurrSelectRes.Face = 0;
		CurrSelectRes.TemplatePointID = 0;
		CurrSelectRes.TargetPointID = 0;
		CurrSelectRes.SurfacePoint = Vector3f::Zero();
	}//lmbEmptySelection

	void ViewportInputManager::processManualCorrespondenceCreation(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries,
		CorrespondenceStorage& CorrStorage) {

		if (m_ManualCorrData.VertexVertexPair) {
			CorrStorage.addCorrespondence(m_ManualCorrData.Type, m_ManualCorrData.TemplatePointUID, m_ManualCorrData.TargetPointUID);
		}
		else {
			CorrStorage.addCorrespondence(m_ManualCorrData.Type, m_ManualCorrData.TemplatePointUID, DatasetGeometries.at(DatasetType::TARGET), m_ManualCorrData.TargetFaceID,
				m_ManualCorrData.TargetPointPos, m_ManualCorrData.TargetPointBaryCoords);
		}

		// -> selection markers no longer needed
		for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) {
			VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TEMPLATE, false);
			VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::DTEMPLATE, false);
			VPRMgr.showDefaultSelectMarkerOnDataset(VP, DatasetType::TARGET, false);
		}

		// -> replace temporary markers of manual correspondence with actual correspondence markers
		CorrespondenceStorage::Correspondence StoredCorr;
		CorrStorage.correspondenceAtTemplateVertex(StoredCorr, m_ManualCorrData.TemplatePointUID);
		VPRMgr.addCorrMarkerTemplate(StoredCorr.TemplatePointUID, DatasetGeometries.at(DatasetType::TEMPLATE).vertex(m_ManualCorrData.TemplatePointUID));
		VPRMgr.addCorrMarkerTarget(StoredCorr.TargetPointUID, m_ManualCorrData.TargetPointPos);

		// clear manual correspondence data
		m_ManualCorrData.TemplatePointUID = 0;
		m_ManualCorrData.TargetPointUID = 0;
		m_ManualCorrData.TargetFaceID = 0;
		m_ManualCorrData.TargetPointPos = Vector3f::Zero();
		m_ManualCorrData.TargetPointBaryCoords = Vector3f::Zero();
		m_ManualCorrData.VertexVertexPair = false;
		m_ManualCorrData.Type = CorrespondenceType::NONE;

		m_ManualCorrTemplateReady = false;
		m_ManualCorrTargetReady = false;

		// empty selection data of this frame
		CurrSelectRes.ViewportID = 0;
		CurrSelectRes.Dataset = DatasetType::NONE;
		CurrSelectRes.ResultType = PickingResultType::NONE;
		CurrSelectRes.Face = 0;
		CurrSelectRes.TemplatePointID = 0;
		CurrSelectRes.TargetPointID = 0;
		CurrSelectRes.SurfacePoint = Vector3f::Zero();
	}//processManualCorrespondenceCreation

	void ViewportInputManager::processMMBDown(
		CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, Vector2f CurrentCursorPosOGL, int32_t ViewportUnderMouse, bool CaptureKeyboard) {

		GlobalAppState.currMMBCursorPos(CurrentCursorPosOGL);
		if (GlobalAppState.prevMouseButtonState(CForge::Mouse::BTN_MIDDLE) == GLFW_RELEASE) GlobalAppState.prevMMBCursorPos(GlobalAppState.currMMBCursorPos());

		if (GlobalAppState.mouseButtonViewportFocus() > -1) {
			const auto VPFocus = GlobalAppState.mouseButtonViewportFocus();
			if (RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_LEFT_SHIFT)) //TODO: change to left alt
				VPRMgr.acrballRotateAllViewports(VPFocus, GlobalAppState.prevMMBCursorPos(), GlobalAppState.currMMBCursorPos());
			else
				VPRMgr.viewportArcballRotate(VPFocus, GlobalAppState.prevMMBCursorPos(), GlobalAppState.currMMBCursorPos());
		}

		GlobalAppState.prevMMBCursorPos(GlobalAppState.currMMBCursorPos());
	}//processMMBDown

	void ViewportInputManager::processMMBUp(TempRegAppState& GlobalAppState) {
		GlobalAppState.currMMBCursorPos(Vector2f::Zero());
		GlobalAppState.prevMMBCursorPos(Vector2f::Zero());
		GlobalAppState.mouseButtonViewportFocus(-1);
	}//processMMBUp

	void ViewportInputManager::processRMBDown(
		CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries,
		CorrespondenceStorage& CorrStorage, Vector2f CurrentCursorPosOGL, int32_t ViewportUnderMouse, bool CaptureKeyboard) {

		//TODO...

	}//processRightMouseButton

	void ViewportInputManager::processMouseHover(
		CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage,
		Vector2f CurrentCursorPosOGL, int32_t ViewportUnderMouse, bool CaptureKeyboard) {

		TempRegAppState::PickingResult CurrHoverRes;
		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();

		clearPreviousHoverHighlighting(GlobalAppState, VPRMgr);

		// -> special case on LMB + LEFT ALT: only allow highlighting of complete correspondence pairs in this frame
		if (!CaptureKeyboard && RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_LEFT_ALT)) {
			// select existing correspondence at template vertex
			//TODO: create GUI widgets to explicitly choose between creation and deletion of correspondences, so that the selection of existing correspondences can happen 
			//      through all datasets (TEMPLATE / DTEMPLATE, TARGET) without interferring with manual correspondence creation or requiring extra keys
			if (RayIntersectRes.Dataset == DatasetType::TEMPLATE || RayIntersectRes.Dataset == DatasetType::DTEMPLATE)
				templateCorrPairHovering(CurrHoverRes, GlobalAppState, VPRMgr, ViewportUnderMouse, DatasetGeometries.at(DatasetType::TEMPLATE), CorrStorage);
			GlobalAppState.prevHoverRes(CurrHoverRes);
			return;
		}

		if (RayIntersectRes.Dataset == DatasetType::TEMPLATE || RayIntersectRes.Dataset == DatasetType::DTEMPLATE)
			hoverTemplateVertices(CurrHoverRes, GlobalAppState, VPRMgr, ViewportUnderMouse, DatasetGeometries, CorrStorage);

		if (RayIntersectRes.Dataset == DatasetType::TARGET && RayIntersectRes.GeometryType == DatasetGeometryType::MESH) {
			if (!CaptureKeyboard && RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_LEFT_CONTROL)) {
				hoverTargetPrecisePoints(CurrHoverRes, GlobalAppState, VPRMgr, ViewportUnderMouse, DatasetGeometries, CorrStorage);
			}
			else {
				hoverTargetVertices(CurrHoverRes, GlobalAppState, VPRMgr, ViewportUnderMouse, DatasetGeometries, CorrStorage);
			}
		}

		GlobalAppState.prevHoverRes(CurrHoverRes);
	}//processMouseHover

	void ViewportInputManager::clearPreviousHoverHighlighting(TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr) {
		// undo last frame's state changes to correspondence point /feature point markers, default hover markers, debug markers
		// Has to happen even if ImGui wants to capture mouse / keyboard inputs!

		const auto& PrevHoverRes = GlobalAppState.prevHoverRes();
		const auto& PrevSelectionRes = GlobalAppState.prevSelectRes();

		// mouse hovered over a completed correspondence pair
		if (PrevHoverRes.ResultType == PickingResultType::CORRESPONDENCE_PAIR) {
			// only modify the markers of this correspondence pair if the pair isn't registered as selected (otherwise MarkerMode must remain SELECTED)
			// as target correspondence points can be used multiple times, both markers have to be tested separately
			
			// -> TEMPLATE/DTEMPLATE marker
			if (PrevSelectionRes.TemplatePointID != PrevHoverRes.TemplatePointID)
				VPRMgr.markerModeCorrMarkerTemplate(PrevHoverRes.TemplatePointID, MarkerMode::IDLE);

			// -> TARGET marker
			if (PrevSelectionRes.TargetPointID != PrevHoverRes.TargetPointID)
				VPRMgr.markerModeCorrMarkerTarget(PrevHoverRes.TargetPointID, MarkerMode::IDLE);
		}

		if (PrevHoverRes.ResultType == PickingResultType::CORRESPONDENCE_POINT_VERTEX) {
			if (PrevHoverRes.Dataset == DatasetType::TARGET && (PrevSelectionRes.ResultType != PrevHoverRes.ResultType || PrevSelectionRes.TargetPointID != PrevHoverRes.TargetPointID)) {
				VPRMgr.markerModeCorrMarkerTarget(PrevHoverRes.TargetPointID, MarkerMode::IDLE);
			}
			else if (PrevSelectionRes.ResultType != PrevHoverRes.ResultType || PrevSelectionRes.TemplatePointID != PrevHoverRes.TemplatePointID) {
				VPRMgr.markerModeCorrMarkerTemplate(PrevHoverRes.TemplatePointID, MarkerMode::IDLE);
			}
		}

		if (PrevHoverRes.ResultType == PickingResultType::CORRESPONDENCE_POINT_SURFACE) {
			if (PrevSelectionRes.ResultType != PrevHoverRes.ResultType || PrevSelectionRes.TargetPointID != PrevHoverRes.TargetPointID)
				VPRMgr.markerModeCorrMarkerTarget(PrevHoverRes.TargetPointID, MarkerMode::IDLE);
		}

		//TODO: same for feature point markers

		for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) {
			VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::TEMPLATE, false);
			VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::DTEMPLATE, false);
			VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::TARGET, false);
		}
	}//clearPreviousHoverHighlighting

	void ViewportInputManager::templateCorrPairHovering(TempRegAppState::PickingResult& CurrHoverRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
		DatasetGeometryData& TemplateGeometry, CorrespondenceStorage& CorrStorage) {

		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();

		int64_t PickedVertex = pickVertex(RayIntersectRes, TemplateGeometry, 0.015f);
		if (PickedVertex == -1) return;

		const auto& PrevSelection = GlobalAppState.prevSelectRes();
		if (PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_PAIR && PrevSelection.TemplatePointID == PickedVertex) return; // don't modify the markers of a selected correspondence pair

		CorrespondenceStorage::Correspondence Corr;
		if (CorrStorage.correspondenceAtTemplateVertex(Corr, PickedVertex)) {

			// for testing purposes it is simply assumed that the correspondence at this template vertex is never a feature point pair!
			CurrHoverRes.ViewportID = ViewportUnderMouse;
			CurrHoverRes.Dataset = RayIntersectRes.Dataset;
			CurrHoverRes.ResultType = PickingResultType::CORRESPONDENCE_PAIR;
			CurrHoverRes.TemplatePointID = Corr.TemplatePointUID;
			CurrHoverRes.TargetPointID = Corr.TargetPointUID;
			CurrHoverRes.Face = RayIntersectRes.Face;

			VPRMgr.markerModeCorrMarkerPair(Corr.TemplatePointUID, Corr.TargetPointUID, MarkerMode::HOVERED);
		}
	}//templateCorrPairHovering

	void ViewportInputManager::hoverTemplateVertices(TempRegAppState::PickingResult& CurrHoverRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
		std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage) {

		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();

		int64_t PickedVertex = pickVertex(RayIntersectRes, DatasetGeometries.at(RayIntersectRes.Dataset), 0.015f);
		if (PickedVertex == -1) return; // no vertex in range of cursor

		const auto& PrevSelection = GlobalAppState.prevSelectRes();
		if (PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_PAIR && PrevSelection.TemplatePointID == PickedVertex)
			return; // don't modify the markers of a selected correspondence pair

		if (m_ManualCorrTemplateReady && m_ManualCorrData.TemplatePointUID == PickedVertex)
			return; // don't modify the default selection marker of a manually placed template correspondence point

		CurrHoverRes.ViewportID = ViewportUnderMouse;
		CurrHoverRes.Dataset = DatasetType::TEMPLATE;
		CurrHoverRes.TemplatePointID = PickedVertex;
		CurrHoverRes.Face = RayIntersectRes.Face;

		CorrespondenceStorage::Correspondence Corr;
		if (CorrStorage.correspondenceAtTemplateVertex(Corr, PickedVertex)) {
			// vertex has correspondence data -> modifiy its correspondence marker

			// for testing purposes it is simply assumed that the correspondence at this template vertex is not a feature point pair!
			CurrHoverRes.ResultType = PickingResultType::CORRESPONDENCE_POINT_VERTEX;

			VPRMgr.markerModeCorrMarkerTemplate(CurrHoverRes.TemplatePointID, MarkerMode::HOVERED);

			for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) {
				VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::TEMPLATE, false);
				VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::DTEMPLATE, false);
			}
		}
		else {
			// vertex does not have correspondence data -> handle default hover marker
			CurrHoverRes.ResultType = PickingResultType::MESH_VERTEX;

			VPRMgr.placeDefaultHoverMarker(DatasetType::TEMPLATE, DatasetGeometries.at(DatasetType::TEMPLATE).vertex(CurrHoverRes.TemplatePointID));
			VPRMgr.placeDefaultHoverMarker(DatasetType::DTEMPLATE, DatasetGeometries.at(DatasetType::DTEMPLATE).vertex(CurrHoverRes.TemplatePointID));

			for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP) {
				VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::TEMPLATE, true);
				VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::DTEMPLATE, true);
			}
		}
	}//hoverTemplateVertices

	void ViewportInputManager::hoverTargetVertices(TempRegAppState::PickingResult& CurrHoverRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
		std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage) {

		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();

		int64_t PickedVertex = pickVertex(RayIntersectRes, DatasetGeometries.at(DatasetType::TARGET), 0.015f);
		if (PickedVertex == -1) return;

		const auto& PrevSelection = GlobalAppState.prevSelectRes();
		if (PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_PAIR && PrevSelection.TemplatePointID == PickedVertex)
			return; // don't modify the markers of a selected correspondence pair
		
		if (m_ManualCorrTargetReady && m_ManualCorrData.TargetPointUID == PickedVertex)
			return; // don't modify the default selection marker of a manually placed target correspondence point

		CurrHoverRes.ViewportID = ViewportUnderMouse;
		CurrHoverRes.Dataset = DatasetType::TARGET;
		CurrHoverRes.TargetPointID = PickedVertex;
		CurrHoverRes.Face = RayIntersectRes.Face;

		if (CorrStorage.targetCorrespondencePointExists(PickedVertex)) {
			// vertex has correspondence data -> modifiy its correspondence marker			
			CurrHoverRes.ResultType = PickingResultType::CORRESPONDENCE_POINT_VERTEX;

			VPRMgr.markerModeCorrMarkerTarget(CurrHoverRes.TargetPointID, MarkerMode::HOVERED);

			for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP)
				VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::TARGET, false);
		}
		else {
			// vertex does not have correspondence data -> handle default hover marker for vertices
			CurrHoverRes.ResultType = PickingResultType::MESH_VERTEX;

			VPRMgr.placeDefaultHoverMarker(DatasetType::TARGET, DatasetGeometries.at(DatasetType::TARGET).vertex(CurrHoverRes.TargetPointID));

			for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP)
				VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::TARGET, true);
		}
	}//hoverTargetVertices

	void ViewportInputManager::hoverTargetPrecisePoints(TempRegAppState::PickingResult& CurrHoverRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
		std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage) {

		const auto& RayIntersectRes = GlobalAppState.rayIntersectionResult();

		// check for vertices
		int64_t PickedVertex = pickVertex(RayIntersectRes, DatasetGeometries.at(DatasetType::TARGET), (float)1e-4);
		if (PickedVertex != -1) {

			const auto& PrevSelection = GlobalAppState.prevSelectRes();
			if (PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_PAIR && PrevSelection.TemplatePointID == PickedVertex)
				return; // don't modify the markers of a selected correspondence pair
			
			if (m_ManualCorrTargetReady && m_ManualCorrData.TargetPointUID == PickedVertex)
				return; // don't modify the default selection marker of a manually placed target correspondence point

			CurrHoverRes.ViewportID = ViewportUnderMouse;
			CurrHoverRes.Dataset = DatasetType::TARGET;
			CurrHoverRes.TargetPointID = PickedVertex;
			CurrHoverRes.Face = RayIntersectRes.Face;

			if (CorrStorage.targetCorrespondencePointExists(PickedVertex)) {
				// vertex has correspondence data -> modifiy its correspondence marker			
				CurrHoverRes.ResultType = PickingResultType::CORRESPONDENCE_POINT_VERTEX;

				VPRMgr.markerModeCorrMarkerTarget(CurrHoverRes.TargetPointID, MarkerMode::HOVERED);

				for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP)
					VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::TARGET, false);
			}
			else {
				// vertex does not have correspondence data -> handle default hover marker for vertices
				CurrHoverRes.ResultType = PickingResultType::MESH_VERTEX;

				VPRMgr.placeDefaultHoverMarker(DatasetType::TARGET, DatasetGeometries.at(DatasetType::TARGET).vertex(CurrHoverRes.TargetPointID));

				for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP)
					VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::TARGET, true);
			}
		}
		else {
			// check for correspondence points at arbitrary positions on mesh

			size_t TargetPointID;
			if (CorrStorage.targetCorrespondencePointExists(RayIntersectRes.Face, RayIntersectRes.IntersectionPos, TargetPointID)) {
				// correspondence point found -> modifiy its correspondence marker

				const auto& PrevSelection = GlobalAppState.prevSelectRes();
				if (PrevSelection.ResultType == PickingResultType::CORRESPONDENCE_PAIR && PrevSelection.TargetPointID == TargetPointID)
					return; // don't modify the markers of a selected correspondence pair
				
				if (m_ManualCorrTargetReady && !m_ManualCorrData.VertexVertexPair) {
					if ((m_ManualCorrData.TargetPointPos - RayIntersectRes.IntersectionPos).squaredNorm() < (float)1e-4)
						return; // don't modify the default selection marker of a manually placed target correspondence point
				}

				CurrHoverRes.ViewportID = ViewportUnderMouse;
				CurrHoverRes.Dataset = DatasetType::TARGET;
				CurrHoverRes.ResultType = PickingResultType::CORRESPONDENCE_POINT_SURFACE;
				CurrHoverRes.TargetPointID = TargetPointID;
				CurrHoverRes.Face = RayIntersectRes.Face;

				VPRMgr.markerModeCorrMarkerTarget(CurrHoverRes.TargetPointID, MarkerMode::HOVERED);

				for (size_t VP = 0; VP < VPRMgr.activeViewportCount(); ++VP)
					VPRMgr.showDefaultHoverMarkerOnDataset(VP, DatasetType::TARGET, false);
			}
		}
	}//hoverTargetPrecisePoints

	// raycast against datasets (for picking / highlighting of vertices) if mouse cursor hovers over a viewport
		// only do raycast when: 
		// a) exactly one dataset is active in given viewport or
		// b) mutliple datasets are displayed side by side (NOT layered over each other)
	void ViewportInputManager::raycastDatasets(
		TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, Vector2f& CursorPosOGL, size_t VPIndex) { //TODO: pcl raycasts

		if (VPIndex > VPRMgr.activeViewportCount())
			throw IndexOutOfBoundsExcept("VPIndex");

		TempRegAppState::RayIntersectionResult RayIntersectRes;

		auto ActiveDatasets = VPRMgr.activeDatasetTypes(VPIndex);
		auto ActiveDisplayDataArrangement = VPRMgr.activeDatasetDisplayDataArrangement(VPIndex);
		if (/*ActiveDatasets.size() == 1 ||*/ ActiveDisplayDataArrangement != DisplayDataArrangementMode::LAYERED) {
			for (auto Dataset : ActiveDatasets) {
				auto& Geometry = DatasetGeometries.at(Dataset);
				Vector4f Viewport = Vector4f(VPRMgr.viewportPosition(VPIndex).x(), VPRMgr.viewportPosition(VPIndex).y(), VPRMgr.viewportSize(VPIndex).x(), VPRMgr.viewportSize(VPIndex).y());
				Matrix4f View = VPRMgr.viewportViewMatrix(VPIndex);
				Matrix4f Projection = VPRMgr.viewportProjectionMatrix(VPIndex);
				Matrix4f Model = VPRMgr.datasetModelMatrix(VPIndex, Dataset);

				// pick point on mesh
				if (Geometry.geometryType() == DatasetGeometryType::MESH) {
					int IntersectedFace = -1;
					Vector3f IntersectionPoint = Vector3f::Zero();
					Vector3f BaryCoords = Vector3f::Zero();
					pickPointOnMesh(Geometry, Viewport, Model, View, Projection, CursorPosOGL, IntersectedFace, IntersectionPoint, BaryCoords);

					if (IntersectedFace >= 0) {
						RayIntersectRes.Dataset = Dataset;
						RayIntersectRes.GeometryType = Geometry.geometryType();
						RayIntersectRes.Face = IntersectedFace;
						RayIntersectRes.Vertex = -1;
						RayIntersectRes.IntersectionPos = IntersectionPoint;
						RayIntersectRes.BarycentricCoords = BaryCoords;
						break;
					}
				}
				else { // pick point in point cloud
					//TODO
				}
			}
		}

		GlobalAppState.rayIntersectionResult(RayIntersectRes);
	}//raycastDatasets

	void ViewportInputManager::pickPointOnMesh(
		DatasetGeometryData& Geometry, Vector4f& Viewport, Matrix4f& Model, Matrix4f& View, Matrix4f& Projection, Vector2f& CursorPosOGL, int& IntersectedFace,
		Vector3f& IntersectionPoint, Vector3f& BaryCoords) {

		if (igl::embree::unproject_onto_mesh(
			CursorPosOGL, Geometry.faces(), View * Model, Projection, Viewport,
			Geometry.embreeIntersector(), IntersectedFace, BaryCoords)) {
			// calculate picked vertex, if any are within range of ray<->mesh intersection point
			Vector3i Face = Geometry.face(IntersectedFace);
			Vector3f V0 = Geometry.vertex(Face(0));
			Vector3f V1 = Geometry.vertex(Face(1));
			Vector3f V2 = Geometry.vertex(Face(2));
			IntersectionPoint = (BaryCoords.x() * V0) + (BaryCoords.y() * V1) + (BaryCoords.z() * V2);
		}
	}//pickPointOnMesh

	int64_t ViewportInputManager::pickVertex(const TempRegAppState::RayIntersectionResult& RayIntersectRes, const DatasetGeometryData& Geometry, float Tolerance) {
		Vector3i Face = Geometry.face(RayIntersectRes.Face);
		Vector3f V0 = Geometry.vertex(Face(0));
		Vector3f V1 = Geometry.vertex(Face(1));
		Vector3f V2 = Geometry.vertex(Face(2));

		float DistToV0 = (V0 - RayIntersectRes.IntersectionPos).squaredNorm();
		float DistToV1 = (V1 - RayIntersectRes.IntersectionPos).squaredNorm();
		float DistToV2 = (V2 - RayIntersectRes.IntersectionPos).squaredNorm();

		float DistToClosest = DistToV0;
		size_t ClosestVertex = Face(0);

		if (DistToV1 < DistToClosest) {
			DistToClosest = DistToV1;
			ClosestVertex = Face(1);
		}

		if (DistToV2 < DistToClosest) {
			DistToClosest = DistToV2;
			ClosestVertex = Face(2);
		}

		int64_t PickedVertex = -1;

		if (DistToClosest < Tolerance)
			PickedVertex = ClosestVertex;

		return PickedVertex;
	}//pickVertex
}