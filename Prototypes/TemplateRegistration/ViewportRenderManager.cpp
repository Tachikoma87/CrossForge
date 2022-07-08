#include "ViewportRenderManager.h"

#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../Examples/SceneUtilities.hpp"

namespace TempReg {
	
	ViewportRenderManager::ViewportRenderManager(size_t MaxViewportCount) :
		m_MaxViewportCount(MaxViewportCount), m_ActiveViewportArrangement(ViewportArrangementType::ONE_FULLSCREEN) {

		m_GlobalState.DisplayCorrespondences = false;
		m_GlobalState.GlobalMarkers = false;
		m_GlobalState.DefaultTemplateColor = Vector3f(0.819f, 0.819f, 0.819f);	// light grey #d1d1d1
		m_GlobalState.DefaultTargetColor = Vector3f(0.635f, 0.788f, 0.964f);	// light blue #a2c9f6
	}//Constructor

	ViewportRenderManager::~ViewportRenderManager() {
		for (auto& it : m_DatasetModels) it.second.clear();		
		m_DatasetModels.clear();

		// clear all viewports
		for (size_t i = 0; i < m_Viewports.size(); ++i) {
			if (m_Viewports[i] == nullptr) continue;

			clearDatasetDisplayData(i);

			m_Viewports[i]->RootSGN.clear();
			m_Viewports[i]->SG.clear();
			m_Viewports[i]->Cam.clear();

			delete m_Viewports[i];
			m_Viewports[i] = nullptr;
		}
		m_Viewports.clear();

		// clear all marker data
	
		for (auto& M : m_DEBUG_MeshMarkers) M.second.clear();
		m_DEBUG_MeshMarkers.clear();

		for (auto& M : m_DefaultSelectMarkers) M.second.clear();
		m_DefaultSelectMarkers.clear();

		for (auto& M : m_DefaultHoverMarkers) M.second.clear();
		m_DefaultHoverMarkers.clear();

		for (auto& Cloud : m_FeatMarkerClouds) Cloud.second.clear();
		m_FeatMarkerClouds.clear();

		for (auto& Cloud : m_CorrMarkerClouds) Cloud.second.clear();
		m_CorrMarkerClouds.clear();

		for (auto& MA : m_MarkerActors) MA.second->release();
		m_MarkerActors.clear();
	}//Destructor

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Viewport interactions
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<Vector4f> ViewportRenderManager::calculateViewportTiling(ViewportArrangementType ArrangementType, Vector4f ContentArea) {
		std::vector<Vector4f> Tiles;
		Vector4f Tile0 = Vector4f::Zero();
		Vector4f Tile1 = Vector4f::Zero();
		Vector4f Tile2 = Vector4f::Zero();
		Vector4f Tile3 = Vector4f::Zero();

		switch (ArrangementType) {
		case ViewportArrangementType::ONE_FULLSCREEN: {
			Tile0(0) = ContentArea(0); // Pos.x
			Tile0(1) = ContentArea(1); // Pos.y
			Tile0(2) = ContentArea(2); // Size.x
			Tile0(3) = ContentArea(3); // Size.y

			Tiles.push_back(Tile0);

			break;
		}
		case ViewportArrangementType::TWO_COLUMNS: {
			Tile0(0) = ContentArea(0); // Pos0.x
			Tile0(1) = ContentArea(1); // Pos0.y
			Tile0(2) = ContentArea(2) / 2.0f; // Size0.x
			Tile0(3) = ContentArea(3); // Size0.y

			Tile1(0) = ContentArea(0) + (ContentArea(2) / 2.0f); // Pos1.x
			Tile1(1) = ContentArea(1); // Pos1.y
			Tile1(2) = ContentArea(2) / 2.0f; // Size1.x
			Tile1(3) = ContentArea(3); // Size1.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);

			break;
		}
		case ViewportArrangementType::THREE_BIGTOP: {
			Tile0(0) = ContentArea(0); // Pos0.x
			Tile0(1) = ContentArea(1) + (ContentArea(3) / 2.0f); // Pos0.y
			Tile0(2) = ContentArea(2); // Size0.x
			Tile0(3) = ContentArea(3) / 2.0f; // Size0.y

			Tile1(0) = ContentArea(0); // Pos1.x
			Tile1(1) = ContentArea(1); // Pos1.y
			Tile1(2) = ContentArea(2) / 2.0f; // Size1.x
			Tile1(3) = ContentArea(3) / 2.0f; // Size1.y

			Tile2(0) = ContentArea(0) + (ContentArea(2) / 2.0f); // Pos2.x
			Tile2(1) = ContentArea(1); // Pos2.y
			Tile2(2) = ContentArea(2) / 2.0f; // Size2.x
			Tile2(3) = ContentArea(3) / 2.0f; // Size2.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);
			Tiles.push_back(Tile2);

			break;
		}
		case ViewportArrangementType::THREE_BIGBOTTOM: {
			Tile0(0) = ContentArea(0); // Pos.x
			Tile0(1) = ContentArea(1) + (ContentArea(3) / 2.0f); // Pos.y
			Tile0(2) = ContentArea(2) / 2.0f; // Size.x
			Tile0(3) = ContentArea(3) / 2.0f; // Size.y

			Tile1(0) = ContentArea(0) + (ContentArea(2) / 2.0f); // Pos.x
			Tile1(1) = ContentArea(1) + (ContentArea(3) / 2.0f); // Pos.y
			Tile1(2) = ContentArea(2) / 2.0f; // Size.x
			Tile1(3) = ContentArea(3) / 2.0f; // Size.y

			Tile2(0) = ContentArea(0); // Pos.x
			Tile2(1) = ContentArea(1); // Pos.y
			Tile2(2) = ContentArea(2); // Size.x
			Tile2(3) = ContentArea(3) / 2.0f; // Size.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);
			Tiles.push_back(Tile2);

			break;
		}
		case ViewportArrangementType::THREE_BIGLEFT: {
			Tile0(0) = ContentArea(0); // Pos.x
			Tile0(1) = ContentArea(1); // Pos.y
			Tile0(2) = ContentArea(2) / 2.0f; // Size.x
			Tile0(3) = ContentArea(3); // Size.y

			Tile1(0) = ContentArea(0) + (ContentArea(2) / 2.0f); // Pos.x
			Tile1(1) = ContentArea(1) + (ContentArea(3) / 2.0f); // Pos.y
			Tile1(2) = ContentArea(2) / 2.0f; // Size.x
			Tile1(3) = ContentArea(3) / 2.0f; // Size.y

			Tile2(0) = ContentArea(0) + (ContentArea(2) / 2.0f); // Pos.x
			Tile2(1) = ContentArea(1); // Pos.y
			Tile2(2) = ContentArea(2) / 2.0f; // Size.x
			Tile2(3) = ContentArea(3) / 2.0f; // Size.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);
			Tiles.push_back(Tile2);

			break;
		}
		case ViewportArrangementType::THREE_BIGRIGHT: {
			Tile0(0) = ContentArea(0); // Pos.x
			Tile0(1) = ContentArea(1) + (ContentArea(3) / 2.0f); // Pos.y
			Tile0(2) = ContentArea(2) / 2.0f; // Size.x
			Tile0(3) = ContentArea(3) / 2.0f; // Size.y

			Tile1(0) = ContentArea(0); // Pos.x
			Tile1(1) = ContentArea(1); // Pos.y
			Tile1(2) = ContentArea(2) / 2.0f; // Size.x
			Tile1(3) = ContentArea(3) / 2.0f; // Size.y

			Tile2(0) = ContentArea(0) + (ContentArea(2) / 2.0f); // Pos.x
			Tile2(1) = ContentArea(1); // Pos.y
			Tile2(2) = ContentArea(2) / 2.0f; // Size.x
			Tile2(3) = ContentArea(3); // Size.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);
			Tiles.push_back(Tile2);

			break;
		}
		case ViewportArrangementType::THREE_COLUMNS: {
			Tile0(0) = ContentArea(0); // Pos.x
			Tile0(1) = ContentArea(1); // Pos.y
			Tile0(2) = ContentArea(2) / 3.0f; // Size.x
			Tile0(3) = ContentArea(3); // Size.y

			Tile1(0) = ContentArea(0) + (ContentArea(2) / 3.0f); // Pos.x
			Tile1(1) = ContentArea(1); // Pos.y
			Tile1(2) = ContentArea(2) / 3.0f; // Size.x
			Tile1(3) = ContentArea(3); // Size.y

			Tile2(0) = ContentArea(0) + ((ContentArea(2) / 3.0f) * 2.0f); // Pos.x
			Tile2(1) = ContentArea(1); // Pos.y
			Tile2(2) = ContentArea(2) / 3.0f; // Size.x
			Tile2(3) = ContentArea(3); // Size.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);
			Tiles.push_back(Tile2);

			break;
		}
		case ViewportArrangementType::THREE_EVEN: {
			Tile0(0) = ContentArea(0); // Pos.x
			Tile0(1) = ContentArea(1) + (ContentArea(3) / 2.0f); // Pos.y
			Tile0(2) = ContentArea(2) / 2.0f; // Size.x
			Tile0(3) = ContentArea(3) / 2.0f; // Size.y

			Tile1(0) = ContentArea(0) + (ContentArea(2) / 2.0f); // Pos.x
			Tile1(1) = ContentArea(1) + (ContentArea(3) / 2.0f); // Pos.y
			Tile1(2) = ContentArea(2) / 2.0f; // Size.x
			Tile1(3) = ContentArea(3) / 2.0f; // Size.y

			Tile2(0) = ContentArea(0); // Pos.x
			Tile2(1) = ContentArea(1); // Pos.y
			Tile2(2) = ContentArea(2) / 2.0f; // Size.x
			Tile2(3) = ContentArea(3) / 2.0f; // Size.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);
			Tiles.push_back(Tile2);

			break;
		}
		case ViewportArrangementType::FOUR_EVEN: {
			Tile0(0) = ContentArea(0); // Pos.x
			Tile0(1) = ContentArea(1) + (ContentArea(3) / 2.0f); // Pos.y
			Tile0(2) = ContentArea(2) / 2.0f; // Size.x
			Tile0(3) = ContentArea(3) / 2.0f; // Size.y

			Tile1(0) = ContentArea(0) + (ContentArea(2) / 2.0f); // Pos.x
			Tile1(1) = ContentArea(1) + (ContentArea(3) / 2.0f); // Pos.y
			Tile1(2) = ContentArea(2) / 2.0f; // Size.x
			Tile1(3) = ContentArea(3) / 2.0f; // Size.y

			Tile2(0) = ContentArea(0); // Pos.x
			Tile2(1) = ContentArea(1); // Pos.y
			Tile2(2) = ContentArea(2) / 2.0f; // Size.x
			Tile2(3) = ContentArea(3) / 2.0f; // Size.y

			Tile3(0) = ContentArea(0) + (ContentArea(2) / 2.0f); // Pos.x
			Tile3(1) = ContentArea(1); // Pos.y
			Tile3(2) = ContentArea(2) / 2.0f; // Size.x
			Tile3(3) = ContentArea(3) / 2.0f; // Size.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);
			Tiles.push_back(Tile2);
			Tiles.push_back(Tile3);

			break;
		}
		}

		return Tiles;
	}//calculateViewportTiling

	void ViewportRenderManager::loadViewports(const std::vector<Vector4f>& Tiles, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries) {
		
		Vector4i Tile = Vector4i::Zero();
		size_t i = 0;
		size_t ActiveVPCount = activeViewportCount();

		// reuse existing viewports
		while (i < std::min(ActiveVPCount, Tiles.size())) {
			Tile = Tiles[i].array().ceil().cast<int>();
			m_Viewports[i]->VP.Position(0) = Tile(0);
			m_Viewports[i]->VP.Position(1) = Tile(1);
			m_Viewports[i]->VP.Size(0) = Tile(2);
			m_Viewports[i]->VP.Size(1) = Tile(3);
			m_Viewports[i]->Cam.resetToOrigin();
			m_Viewports[i]->Cam.position(Vector3f(0.0f, 0.0f, 15.0f));
			m_Viewports[i]->Cam.projectionMatrix(m_Viewports[i]->VP.Size.x(), m_Viewports[i]->VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);
			++i;
		}

		// add missing viewports
		while (i < Tiles.size()) {
			Tile = Tiles[i].array().ceil().cast<int>();
			addViewport(Tile(0), Tile(1), Tile(2), Tile(3), DatasetGeometries);			
			++i;
		}

		//remove spare viewports
		while (i < ActiveVPCount) {
			clearDatasetDisplayData(i);

			m_Viewports[i]->RootSGN.clear();
			m_Viewports[i]->SG.clear();

			delete m_Viewports[i];
			m_Viewports[i] = nullptr;
			++i;
		}
	}//loadViewports

	void ViewportRenderManager::resizeActiveViewports(const std::vector<Vector4f>& Tiles) {
		if (Tiles.size() != activeViewportCount()) throw IndexOutOfBoundsExcept("Tiles.size() != activeViewportCount()");

		Vector4i Tile = Vector4i::Zero();
		
		for (size_t i = 0; i < Tiles.size(); ++i) {
			Tile = Tiles[i].array().ceil().cast<int>();
			m_Viewports[i]->VP.Position(0) = Tile(0);
			m_Viewports[i]->VP.Position(1) = Tile(1);
			m_Viewports[i]->VP.Size(0) = Tile(2);
			m_Viewports[i]->VP.Size(1) = Tile(3);
			m_Viewports[i]->Cam.projectionMatrix(m_Viewports[i]->VP.Size.x(), m_Viewports[i]->VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);
		}
	}//resizeActiveViewports

	size_t ViewportRenderManager::activeViewportCount(void) const {
		size_t Count = 0;
		for (size_t i = 0; i < m_Viewports.size(); ++i) {
			if (m_Viewports[i] != nullptr) ++Count;
		}
		return Count;
	}//activeViewportCount

	int32_t ViewportRenderManager::mouseInViewport(Vector2f CursorPosOGL) const {
		for (size_t i = 0; i < activeViewportCount(); ++i) {
			auto pView = m_Viewports[i];
			
			if (CursorPosOGL.x() >= (float)pView->VP.Position.x() && CursorPosOGL.x() <= (float)pView->VP.Position.x() + (float)pView->VP.Size.x()) {
				if (CursorPosOGL.y() >= (float)pView->VP.Position.y() && CursorPosOGL.y() <= (float)pView->VP.Position.y() + (float)pView->VP.Size.y()) {
					return i;
				}
			}
		}
		return -1;
	}//mouseInViewport

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Per viewport interactions
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ViewportRenderManager::updateViewport(size_t VPIndex, float FPSScale) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		
		// process local (per viewport) & global display states...
		auto pView = m_Viewports[VPIndex];
				
		// process markers 
		for (auto& Dataset : pView->DatasetDisplayData) {
			// -> link debug mesh markers to dataset display data of viewport; decide if to show linked markers (TODO comment/remove when done with tests!)
			//m_DEBUG_MeshMarkers.at(Dataset.first).removeFromSceneGraph();
			//m_DEBUG_MeshMarkers.at(Dataset.first).addToSceneGraph(Dataset.second->transformationSGN());
			//m_DEBUG_MeshMarkers.at(Dataset.first).show(pView->DEBUG_ShowDebugMeshMarker.at(Dataset.first));

			// -> link default vertex hover markers to dataset display data of viewport; decide if to show linked markers
			m_DefaultHoverMarkers.at(Dataset.first).removeFromSceneGraph();
			m_DefaultHoverMarkers.at(Dataset.first).addToSceneGraph(Dataset.second->transformationSGN());
			m_DefaultHoverMarkers.at(Dataset.first).show(pView->ShowDefaultHoverMarker.at(Dataset.first));

			// -> link default vertex selection markers to dataset display data of viewport; decide if to show linked markers
			m_DefaultSelectMarkers.at(Dataset.first).removeFromSceneGraph();
			m_DefaultSelectMarkers.at(Dataset.first).addToSceneGraph(Dataset.second->transformationSGN());
			m_DefaultSelectMarkers.at(Dataset.first).show(pView->ShowDefaultSelectMarker.at(Dataset.first));

			// -> link feature pair markers to dataset display data of viewport; decide if to show linked markers
			m_FeatMarkerClouds.at(Dataset.first).removeFromSceneGraph();
			m_FeatMarkerClouds.at(Dataset.first).addToSceneGraph(Dataset.second->transformationSGN());
			m_FeatMarkerClouds.at(Dataset.first).show(pView->ShowFeatMarkers.at(Dataset.first));

			// -> link correspondence pair markers to dataset display data of viewport; decide if to show linked markers
			m_CorrMarkerClouds.at(Dataset.first).removeFromSceneGraph();
			m_CorrMarkerClouds.at(Dataset.first).addToSceneGraph(Dataset.second->transformationSGN());
			m_CorrMarkerClouds.at(Dataset.first).show(pView->ShowCorrMarkers.at(Dataset.first));
		}

		// update the scene graph for this viewport
		pView->SG.update(FPSScale);
	}//updateViewport

	void ViewportRenderManager::renderViewport(size_t VPIndex, CForge::RenderDevice& RDev) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->SG.render(&RDev);
	}//renderViewport

	Vector2i ViewportRenderManager::viewportPosition(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");
		return m_Viewports[VPIndex]->VP.Position;
	}//viewportPosition

	Vector2i ViewportRenderManager::viewportSize(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");
		return m_Viewports[VPIndex]->VP.Size;
	}//viewportSize

	Vector2f ViewportRenderManager::viewportCenter(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");

		Vector2f Pos((float)m_Viewports[VPIndex]->VP.Position.x(), (float)m_Viewports[VPIndex]->VP.Position.y());
		Vector2f Size((float)m_Viewports[VPIndex]->VP.Size.x(), (float)m_Viewports[VPIndex]->VP.Size.y());
		return (Pos + (Size / 2.0f));
	}//viewportCenter

	CForge::VirtualCamera* ViewportRenderManager::viewportCam(size_t VPIndex) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return &m_Viewports[VPIndex]->Cam;
	}//viewportCam

	CForge::RenderDevice::Viewport ViewportRenderManager::viewportGetRenderDeviceViewport(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");
		return m_Viewports[VPIndex]->VP;
	}//viewportGetRenderDeviceViewport
	
	void ViewportRenderManager::viewportSetCamProjectionMode(size_t VPIndex, ViewportProjectionMode Mode) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->ProjMode = Mode;
	}//viewportSetCamProjectionMode

	void ViewportRenderManager::viewportArcballRotate(size_t VPIndex, Vector2f Start, Vector2f End) {//TODO
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		Quaternionf Rotation = arcballRotation(VPIndex, Start, End);

		for (auto& Dataset : m_Viewports[VPIndex]->DatasetDisplayData)
			Dataset.second->rotation(Rotation * Dataset.second->rotation());
	}//viewportArcballRotate

	void ViewportRenderManager::acrballRotateAllViewports(size_t VPIndex, Vector2f Start, Vector2f End) {//TODO
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		Quaternionf Rotation = arcballRotation(VPIndex, Start, End);

		for (auto pView : m_Viewports) {
			if (pView == nullptr) continue;
			for (auto& Dataset : pView->DatasetDisplayData)
				Dataset.second->rotation(Rotation * Dataset.second->rotation());
		}
	}//acrballRotateAllViewports

	void ViewportRenderManager::viewportSetCamPos(size_t VPIndex, Vector3f Eye) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitEye = Eye;
	}//viewportSetCamPos

	void ViewportRenderManager::viewportSetCamTarget(size_t VPIndex, Vector3f Target) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitTarget = Target;
	}//viewportSetCamTarget

	void ViewportRenderManager::viewportSetCamZoom(size_t VPIndex, float Zoom) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitZoom = Zoom;
	}//viewportSetCamZoom

	void ViewportRenderManager::viewportMoveCamPos(size_t VPIndex, Vector3f Movement) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitEye += Movement;
	}//viewportMoveCamPos

	void ViewportRenderManager::viewportMoveCamTarget(size_t VPIndex, Vector3f Movement) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitTarget += Movement;
	}//viewportMoveCamTarget

	void ViewportRenderManager::viewportZoomCam(size_t VPIndex, float Change) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitZoom += Change;
	}//viewportZoomCam

	ViewportProjectionMode ViewportRenderManager::viewportCamProjectionMode(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->ProjMode;
	}//viewportCamProjectionMode

	Matrix4f ViewportRenderManager::viewportViewMatrix(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->Cam.cameraMatrix();
	}//viewportViewMatrix

	Matrix4f ViewportRenderManager::viewportProjectionMatrix(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->Cam.projectionMatrix();
	}//viewportProjectionMatrix

	Vector3f ViewportRenderManager::viewportCamPos(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->OrbitEye;
	}//viewportCamPos

	Vector3f ViewportRenderManager::viewportCamTarget(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->OrbitTarget;
	}//viewportCamTarget

	float ViewportRenderManager::viewportCamZoom(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->OrbitZoom;
	}//viewportCamZoom

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Dataset interactions
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ViewportRenderManager::initDatasetModelFromFile(DatasetType DT, std::string Filepath) {
		auto Res = m_DatasetModels.insert(std::pair<DatasetType, CForge::T3DMesh<float>>(DT, CForge::T3DMesh<float>()));

		if (!Res.second) {
			std::string Type;
			if (DT == DatasetType::TEMPLATE) Type = "TEMPLATE";
			if (DT == DatasetType::DTEMPLATE) Type = "DTEMPLATE";
			if (DT == DatasetType::TARGET) Type = "TARGET";
			throw CForgeExcept("Dataset " + Type + " already exists in m_DatasetModels");
		}

		CForge::SAssetIO::load(Filepath, &m_DatasetModels.at(DT));
		CForge::SceneUtilities::setMeshShader(&m_DatasetModels.at(DT), 0.4f, 0.0f);
		m_DatasetModels.at(DT).computePerVertexNormals();
		m_DatasetModels.at(DT).computeAxisAlignedBoundingBox();
	}//initDatasetModelFromFile

	void ViewportRenderManager::updateDatasetModel(DatasetType DT/*, new vertices here...*/) { //TODO
		if (m_DatasetModels.count(DT) < 1) {
			std::string Name;
			if (DT == DatasetType::TEMPLATE) Name = "TEMPLATE";
			if (DT == DatasetType::DTEMPLATE) Name = "DTEMPLATE";
			if (DT == DatasetType::TARGET) Name = "TARGET";
			throw CForgeExcept("Dataset model " + Name + " does not exist!");
		}

		//TODO...

		m_DatasetModels.at(DT).computePerVertexNormals();
		m_DatasetModels.at(DT).computeAxisAlignedBoundingBox();
	}//updateDatasetModel

	void ViewportRenderManager::updateDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData>::iterator itDatasetGeom) { //TODO
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		
		auto pView = m_Viewports[VPIndex];

		//TODO...

	}//updateDatasetDisplayData

	void ViewportRenderManager::showDatasetDisplayData(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->DatasetDisplayData.at(DT)->show(Show);
	}//showDatasetDisplayData

	void ViewportRenderManager::enableWireframeActor(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->DatasetDisplayData.at(DT)->showWireframe(Show);
	}//enableWireframeActor

	void ViewportRenderManager::enablePrimitivesActor(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->DatasetDisplayData.at(DT)->showPrimitives(Show);
	}//enablePrimitivesActor

	void ViewportRenderManager::setSolidColorShading(size_t VPIndex, DatasetType DT, bool PrimitivesActor, Vector3f* Color) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("DatasetType not initialized");

		auto pDisplayData = m_Viewports[VPIndex]->DatasetDisplayData.at(DT);
		Vector3f C;
		
		if (Color != nullptr) C = *Color;
		else C = (DT == DatasetType::TARGET) ? m_GlobalState.DefaultTargetColor : m_GlobalState.DefaultTemplateColor;

		// update DatasetActor specified by <PrimitivesActor>
		if (PrimitivesActor) pDisplayData->primitivesColor(&m_DatasetModels.at(DT), C);
		else pDisplayData->wireframeColor(&m_DatasetModels.at(DT), C);
	}//setSolidColorShading
	
	//TODO: change to support pPrimitivesActor and pWireframeActor!
	//void ViewportRenderManager::setHausdorffDistColorShading(size_t VPIndex, DatasetType DT, std::vector<float>& HausdorffVals) {
	//			
	//	if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
	//	if (HausdorffVals.size() != m_DatasetModels.at(DT).vertexCount()) throw CForgeExcept("Size mismatch: VertexColors.size() != m_DatasetModels.at(DT).vertexCount()");
	//	
	//	auto pView = m_Viewports[VPIndex];

	//	auto VertexColors = calculateHausdorffVertexColors(HausdorffVals); //TODO
	//	m_DatasetModels.at(DT).colors(&VertexColors);

	//	//update DatasetActor
	//	auto* DisplayData = pView->DatasetDisplayData.at(DT);
	//	DatasetRenderMode RM = pView->DatasetDisplayData.at(DT)->pDatasetActor->renderMode();

	//	//DisplayData->TransSGN.removeChild(&(DisplayData->DatasetGeomSGN));
	//	//DisplayData->DatasetGeomSGN.clear();
	//	DisplayData->pDatasetActor->release();
	//	
	//	DisplayData->pDatasetActor = new DatasetActor();
	//	DisplayData->pDatasetActor->init(&m_DatasetModels.at(DT), RM);
	//	//DisplayData->DatasetGeomSGN.init(&(DisplayData->TransSGN), DisplayData->pDatasetActor);
	//	DisplayData->DatasetGeomSGN.actor(DisplayData->pDatasetActor);
	//}//setHausdorffDistColorShading

	void ViewportRenderManager::setDatasetDisplayDataArrangement(size_t VPIndex, DisplayDataArrangementMode Arrangement) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto pView = m_Viewports[VPIndex];
		pView->DisplayDataArrangement = Arrangement;
		if (Arrangement == DisplayDataArrangementMode::LAYERED) arrangeDatasetDisplayDataLayered(VPIndex);
		else arrangeDatasetDisplayDataSideBySide(VPIndex);
	}//setDatasetDisplayDataArrangement

	void ViewportRenderManager::arrangeDatasetDisplayDataLayered(size_t VPIndex) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto pView = m_Viewports[VPIndex];

		for (auto& D : pView->DatasetDisplayData)
			D.second->translation(Vector3f::Zero());
		
		pView->DisplayDataArrangement = DisplayDataArrangementMode::LAYERED;
	}//arrangeDatasetDisplayDataLayered

	void ViewportRenderManager::arrangeDatasetDisplayDataSideBySide(size_t VPIndex) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto pView = m_Viewports[VPIndex];

		// update positions of all existing datasets
		auto pTemplateDisplayData = pView->DatasetDisplayData.at(DatasetType::TEMPLATE);
		auto pDTemplateDisplayData = pView->DatasetDisplayData.at(DatasetType::DTEMPLATE);
		auto pTargetDisplayData = pView->DatasetDisplayData.at(DatasetType::TARGET);
		float XWidthTotal = 0.0f;
		float MaxExtentTemplate = -1.0f;
		float MaxExtentDTemplate = -1.0f;
		float MaxExtentTarget = -1.0f;
		
		// -> sum up biggest extents of bounding box of each displayed dataset

		if (pTemplateDisplayData->shown()) {
			auto& Model = m_DatasetModels.at(DatasetType::TEMPLATE);
			Vector3f Extents = (Model.aabb().Max - Model.aabb().Min).cwiseAbs();
			MaxExtentTemplate = Extents(0);
			if (Extents(1) > MaxExtentTemplate) MaxExtentTemplate = Extents(1);
			if (Extents(2) > MaxExtentTemplate) MaxExtentTemplate = Extents(2);
			XWidthTotal += MaxExtentTemplate;
		}
		
		if (pDTemplateDisplayData->shown()) {
			auto& Model = m_DatasetModels.at(DatasetType::DTEMPLATE);
			Vector3f Extents = (Model.aabb().Max - Model.aabb().Min).cwiseAbs();
			MaxExtentDTemplate = Extents(0);
			if (Extents(1) > MaxExtentDTemplate) MaxExtentDTemplate = Extents(1);
			if (Extents(2) > MaxExtentDTemplate) MaxExtentDTemplate = Extents(2);
			XWidthTotal += MaxExtentDTemplate;
		}
		
		if (pTargetDisplayData->shown()) {
			auto& Model = m_DatasetModels.at(DatasetType::TARGET);
			Vector3f Extents = (Model.aabb().Max - Model.aabb().Min).cwiseAbs();
			MaxExtentTarget = Extents(0);
			if (Extents(1) > MaxExtentTarget) MaxExtentTarget = Extents(1);
			if (Extents(2) > MaxExtentTarget) MaxExtentTarget = Extents(2);
			XWidthTotal += MaxExtentTarget;
		}
		
		// -> center arrangement on world space origin
		float XStart = -(XWidthTotal * 0.5f);
				
		if (pTemplateDisplayData->shown()) {
			auto BB = m_DatasetModels.at(DatasetType::TEMPLATE).aabb();
			Vector3f Position = Vector3f(XStart + (MaxExtentTemplate * 0.5f), 0.0f, 0.0f);
			pTemplateDisplayData->translation(Position);
			XStart += MaxExtentTemplate;
		}
				
		if (pDTemplateDisplayData->shown()) {
			auto BB = m_DatasetModels.at(DatasetType::DTEMPLATE).aabb();
			Vector3f Position = Vector3f(XStart + (MaxExtentDTemplate * 0.5f), 0.0f, 0.0f);
			pDTemplateDisplayData->translation(Position);
			XStart += MaxExtentDTemplate;
		}
				
		if (pTargetDisplayData->shown()) {
			auto BB = m_DatasetModels.at(DatasetType::TARGET).aabb();
			Vector3f Position = Vector3f(XStart + (MaxExtentTarget * 0.5f), 0.0f, 0.0f);
			pTargetDisplayData->translation(Position);
		}

		pView->DisplayDataArrangement = DisplayDataArrangementMode::SIDE_BY_SIDE;
	}//arrangeDatasetDispalyDataSideBySide

	DisplayDataArrangementMode ViewportRenderManager::activeDatasetDisplayDataArrangement(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->DisplayDataArrangement;
	}//activeDatasetDisplayDataArrangement

	std::vector<DatasetType> ViewportRenderManager::activeDatasetTypes(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto pView = m_Viewports[VPIndex];
		std::vector<DatasetType> ActiveTypes;

		for (auto it = pView->DatasetDisplayData.begin(); it != pView->DatasetDisplayData.end(); ++it)
			if (it->second->shown()) ActiveTypes.push_back(it->first);

		return ActiveTypes;
	}//activeDatasetTypes

	Matrix4f ViewportRenderManager::datasetModelMatrix(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		
		auto Dataset = m_Viewports[VPIndex]->DatasetDisplayData.find(DT);
		if (Dataset == m_Viewports[VPIndex]->DatasetDisplayData.end()) {
			std::string Type;
			if (DT == DatasetType::TEMPLATE) Type = "TEMPLATE";
			if (DT == DatasetType::DTEMPLATE) Type = "DTEMPLATE";
			if (DT == DatasetType::TARGET) Type = "TARGET";
			throw CForgeExcept("Dataset " + Type + " not active");
		}

		return Dataset->second->modelMatrix();
	}//datasetModelMatrix

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Vertex marker interactions
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ViewportRenderManager::initMarkerData(std::string Filepath) {
		CForge::T3DMesh<float> RawMesh;
		CForge::SAssetIO::load(Filepath, &RawMesh);
		CForge::SceneUtilities::setMeshShader(&RawMesh, 1.0f, 0.0f);
		RawMesh.computePerVertexNormals();

		Vector3f Color;
		std::vector<Vector3f> Colors;

		// Initialize marker model for MarkerColor::DEFAULT_VERTEX_HOVERING
		Color = Vector3f(0.0f, 0.0f, 0.0f); // black #000000
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors.push_back(Color);
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::DEFAULT_VERTEX_HOVERING, nullptr));
		CForge::StaticActor* pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::DEFAULT_VERTEX_HOVERING) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::DEFAULT_VERTEX_SELECTION
		Color = Vector3f(1.0f, 1.0f, 1.0f); // white #ffffff
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::DEFAULT_VERTEX_SELECTION, nullptr));
		pNewMarkerActor = new CForge::StaticActor(); 
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::DEFAULT_VERTEX_SELECTION) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::FEATURE_IDLE
		Color = Vector3f(0.196f, 0.521f, 0.227f); // dark green #32853a
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::FEATURE_IDLE, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::FEATURE_IDLE) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::FEATURE_HOVERED
		//Color = Vector3f(0.482f, 0.776f, 0.509f); // light green #7bc682
		Color = Vector3f(0.0f, 1.0f, 0.086f); // bright green #00ff16
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::FEATURE_HOVERED, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::FEATURE_HOVERED) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::FEATURE_SELECTED
		Color = Vector3f(0.0f, 1.0f, 0.086f); // bright green #00ff16
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::FEATURE_SELECTED, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::FEATURE_SELECTED) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::CORRESPONDENCE_IDLE
		Color = Vector3f(0.545f, 0.196f, 0.203f); // dark red #8b3234
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::CORRESPONDENCE_IDLE, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::CORRESPONDENCE_HOVERED
		//Color = Vector3f(0.745f, 0.435f, 0.439f); // light red #be6f70
		Color = Vector3f(1.0f, 0.0f, 0.019f); // bright red #ff0005
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::CORRESPONDENCE_HOVERED, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::CORRESPONDENCE_HOVERED) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::CORRESPONDENCE_SELECTED
		Color = Vector3f(1.0f, 0.0f, 0.019f); // bright red #ff0005
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::CORRESPONDENCE_SELECTED, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::CORRESPONDENCE_SELECTED) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::DEBUG
		Color = Vector3f(1.0f, 0.160f, 0.937f); // bright pink #ff29ef
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::DEBUG, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::DEBUG) = pNewMarkerActor;

		Vector3f Scale = Vector3f(0.03f, 0.03f, 0.03f);

		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::DEFAULT_VERTEX_HOVERING);
		m_DefaultHoverMarkers.try_emplace(DatasetType::TEMPLATE);
		m_DefaultHoverMarkers.try_emplace(DatasetType::DTEMPLATE);
		m_DefaultHoverMarkers.try_emplace(DatasetType::TARGET);
		for (auto& M : m_DefaultHoverMarkers) M.second.init(nullptr, pMarkerActor, Vector3f::Zero(), Scale);

		pMarkerActor = m_MarkerActors.at(MarkerColor::DEFAULT_VERTEX_SELECTION);
		m_DefaultSelectMarkers.try_emplace(DatasetType::TEMPLATE);
		m_DefaultSelectMarkers.try_emplace(DatasetType::DTEMPLATE);
		m_DefaultSelectMarkers.try_emplace(DatasetType::TARGET);
		for (auto& M : m_DefaultSelectMarkers) M.second.init(nullptr, pMarkerActor, Vector3f::Zero(), Scale);
			
		pMarkerActor = m_MarkerActors.at(MarkerColor::DEBUG);
		m_DEBUG_MeshMarkers.try_emplace(DatasetType::TEMPLATE);
		m_DEBUG_MeshMarkers.try_emplace(DatasetType::DTEMPLATE);
		m_DEBUG_MeshMarkers.try_emplace(DatasetType::TARGET);
		for (auto& M : m_DEBUG_MeshMarkers) M.second.init(nullptr, pMarkerActor, Vector3f::Zero(), Scale);
		
		m_FeatMarkerClouds.try_emplace(DatasetType::TEMPLATE);
		m_FeatMarkerClouds.try_emplace(DatasetType::DTEMPLATE);
		m_FeatMarkerClouds.try_emplace(DatasetType::TARGET);
		for (auto& Cloud : m_FeatMarkerClouds) Cloud.second.init(Scale);

		m_CorrMarkerClouds.try_emplace(DatasetType::TEMPLATE);
		m_CorrMarkerClouds.try_emplace(DatasetType::DTEMPLATE);
		m_CorrMarkerClouds.try_emplace(DatasetType::TARGET);
		for (auto& Cloud : m_CorrMarkerClouds) Cloud.second.init(Scale);
	}//initMarkerData

	void ViewportRenderManager::placeDefaultHoverMarker(DatasetType DT, const Vector3f VertexPos) {
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_DefaultHoverMarkers.at(DT).translation(VertexPos); // move marker to current vertex position
	}//placeDefaultVertexHoverMarker

	void ViewportRenderManager::placeDefaultSelectMarker(DatasetType DT, const Vector3f VertexPos, MarkerColor MC) {
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_DefaultSelectMarkers.at(DT).actor(m_MarkerActors.at(MC));
		m_DefaultSelectMarkers.at(DT).translation(VertexPos); // move marker to current vertex position
	}//placeDefaultSelectMarker

	bool ViewportRenderManager::showDefaultHoverMarkerOnDataset(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		return m_Viewports[VPIndex]->ShowDefaultHoverMarker.at(DT);
	}//showDefaultVertexHoverMarkerOnDataset

	void ViewportRenderManager::showDefaultHoverMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->ShowDefaultHoverMarker.at(DT) = Show;
	}//showDefaultVertexHoverMarkerOnDataset

	bool ViewportRenderManager::showDefaultSelectMarkerOnDataset(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		return m_Viewports[VPIndex]->ShowDefaultSelectMarker.at(DT);
	}//showDefaultSelectMarkerOnDataset

	void ViewportRenderManager::showDefaultSelectMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->ShowDefaultSelectMarker.at(DT) = Show;
	}//showDefaultSelectMarkerOnDataset

	void ViewportRenderManager::addFeatMarkerTemplate(size_t FeatPointID, const Vector3f MarkerPos) {
		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_IDLE);
		m_FeatMarkerClouds.at(DatasetType::TEMPLATE).addMarkerInstance(FeatPointID, MarkerPos, pMarkerActor);
		m_FeatMarkerClouds.at(DatasetType::DTEMPLATE).addMarkerInstance(FeatPointID, MarkerPos, pMarkerActor);		
	}//addFeatMarkerTemplate
	
	void ViewportRenderManager::addCorrMarkerTemplate(size_t CorrPointID, const Vector3f MarkerPos) {
		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE);
		m_CorrMarkerClouds.at(DatasetType::TEMPLATE).addMarkerInstance(CorrPointID, MarkerPos, pMarkerActor);
		m_CorrMarkerClouds.at(DatasetType::DTEMPLATE).addMarkerInstance(CorrPointID, MarkerPos, pMarkerActor);
	}//addCorrMarkerTemplate

	void ViewportRenderManager::addFeatMarkerTarget(size_t FeatPointID, const Vector3f MarkerPos) {
		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_IDLE);
		m_FeatMarkerClouds.at(DatasetType::TARGET).addMarkerInstance(FeatPointID, MarkerPos, pMarkerActor);
		//markerModeFeatMarkerTarget(FeatPointID, MarkerMode::IDLE); // incase addMarkerInstance(...) returns early because FeatPointID already exists
	}//addFeatMarkerTarget
		
	void ViewportRenderManager::addCorrMarkerTarget(size_t CorrPointID, const Vector3f MarkerPos) {
		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE);
		m_CorrMarkerClouds.at(DatasetType::TARGET).addMarkerInstance(CorrPointID, MarkerPos, pMarkerActor);
		//markerModeCorrMarkerTarget(CorrPointID, MarkerMode::IDLE); // incase addMarkerInstance(...) returns early because CorrPointID already exists
	}//addCorrMarkerTarget

	void ViewportRenderManager::removeFeatMarkerTemplate(size_t FeatPointID) {
		m_FeatMarkerClouds.at(DatasetType::TEMPLATE).removeMarkerInstance(FeatPointID);
		m_FeatMarkerClouds.at(DatasetType::DTEMPLATE).removeMarkerInstance(FeatPointID);
	}//removeFeatMarkerTemplate

	void ViewportRenderManager::removeCorrMarkerTemplate(size_t CorrPointID) {		
		m_CorrMarkerClouds.at(DatasetType::TEMPLATE).removeMarkerInstance(CorrPointID);
		m_CorrMarkerClouds.at(DatasetType::DTEMPLATE).removeMarkerInstance(CorrPointID);
	}//removeCorrMarkerTemplate

	void ViewportRenderManager::removeFeatMarkerTarget(size_t FeatPointID) {
		m_FeatMarkerClouds.at(DatasetType::TARGET).removeMarkerInstance(FeatPointID);
	}//removeFeatMarkerTarget

	void ViewportRenderManager::removeCorrMarkerTarget(size_t CorrPointID) {
		m_CorrMarkerClouds.at(DatasetType::TARGET).removeMarkerInstance(CorrPointID);
	}//removeCorrMarkerTarget

	void ViewportRenderManager::clearFeatMarkers(void) {
		for (auto& Cloud : m_FeatMarkerClouds) Cloud.second.clear();
	}//clearFeatMarkers

	void ViewportRenderManager::clearCorrMarkers(void) {
		for (auto& Cloud : m_CorrMarkerClouds) Cloud.second.clear();
	}//clearCorrMarkers

	void ViewportRenderManager::updateFeatMarkerPosition(DatasetType DT, size_t FeatPointID, const Vector3f MarkerPos) {
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType defined");
		m_FeatMarkerClouds.at(DT).markerPosition(FeatPointID, MarkerPos);
	}//updateFeatMarkerPosition
	
	void ViewportRenderManager::updateCorrMarkerPosition(DatasetType DT, size_t CorrPointID, const Vector3f MarkerPos) {
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType defined");
		m_CorrMarkerClouds.at(DT).markerPosition(CorrPointID, MarkerPos);
	}//updateCorrMarkerPosition

	bool ViewportRenderManager::showFeatMarkersOnDataset(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		return m_Viewports[VPIndex]->ShowFeatMarkers.at(DT);
	}//showFeatMarkersOnDataset
	
	void ViewportRenderManager::showFeatMarkersOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->ShowFeatMarkers.at(DT) = Show;
	}//showFeatMarkersOnDataset
	
	bool ViewportRenderManager::showCorrMarkersOnDataset(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		return m_Viewports[VPIndex]->ShowCorrMarkers.at(DT);
	}//showCorrMarkersOnDataset
	
	void ViewportRenderManager::showCorrMarkersOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->ShowCorrMarkers.at(DT) = Show;
	}//showCorrMarkersOnDataset

	void ViewportRenderManager::markerModeFeatMarkerPair(size_t TemplatePointID, size_t TargetPointID, MarkerMode MM) {
		if (MM == MarkerMode::DEBUG_MESH_MARKER || MM == MarkerMode::NONE) throw CForgeExcept("Invalid MarkerMode");

		CForge::StaticActor* pMarkerActor = nullptr;
		switch (MM) {
		case MarkerMode::IDLE: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_IDLE); break;
		case MarkerMode::HOVERED: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_HOVERED); break;
		case MarkerMode::SELECTED: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_SELECTED); break;
		}
		
		m_FeatMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID, pMarkerActor);
		m_FeatMarkerClouds.at(DatasetType::DTEMPLATE).markerActor(TemplatePointID, pMarkerActor);
		m_FeatMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID, pMarkerActor);
	}//markerModeFeatMarkerPair

	void ViewportRenderManager::markerModeCorrMarkerPair(size_t TemplatePointID, size_t TargetPointID, MarkerMode MM) {
		if (MM == MarkerMode::DEBUG_MESH_MARKER || MM == MarkerMode::NONE) throw CForgeExcept("Invalid MarkerMode");
		
		CForge::StaticActor* pMarkerActor = nullptr;
		switch (MM) {
		case MarkerMode::IDLE: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE); break;
		case MarkerMode::HOVERED: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_HOVERED); break;
		case MarkerMode::SELECTED: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_SELECTED); break;
		}

		m_CorrMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID, pMarkerActor);
		m_CorrMarkerClouds.at(DatasetType::DTEMPLATE).markerActor(TemplatePointID, pMarkerActor);
		m_CorrMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID, pMarkerActor);
	}//markerModeCorrMarkerPair
	
	void ViewportRenderManager::markerModeFeatMarkerTemplate(size_t TemplatePointID, MarkerMode MM) {
		if (MM == MarkerMode::DEBUG_MESH_MARKER || MM == MarkerMode::NONE) throw CForgeExcept("Invalid MarkerMode");

		CForge::StaticActor* pMarkerActor = nullptr;
		switch (MM) {
		case MarkerMode::IDLE: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_IDLE); break;
		case MarkerMode::HOVERED: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_HOVERED); break;
		case MarkerMode::SELECTED: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_SELECTED); break;
		}

		m_CorrMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID, pMarkerActor);
		m_CorrMarkerClouds.at(DatasetType::DTEMPLATE).markerActor(TemplatePointID, pMarkerActor);
	}//markerModeFeatMarkerTemplate
	
	void ViewportRenderManager::markerModeCorrMarkerTemplate(size_t TemplatePointID, MarkerMode MM) {
		if (MM == MarkerMode::DEBUG_MESH_MARKER || MM == MarkerMode::NONE) throw CForgeExcept("Invalid MarkerMode");

		CForge::StaticActor* pMarkerActor = nullptr;
		switch (MM) {
		case MarkerMode::IDLE: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE); break;
		case MarkerMode::HOVERED: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_HOVERED); break;
		case MarkerMode::SELECTED: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_SELECTED); break;
		}

		m_CorrMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID, pMarkerActor);
		m_CorrMarkerClouds.at(DatasetType::DTEMPLATE).markerActor(TemplatePointID, pMarkerActor);
	}//markerModeCorrMarkerTemplate
	
	void ViewportRenderManager::markerModeFeatMarkerTarget(size_t TargetPointID, MarkerMode MM) {
		if (MM == MarkerMode::DEBUG_MESH_MARKER || MM == MarkerMode::NONE) throw CForgeExcept("Invalid MarkerMode");

		CForge::StaticActor* pMarkerActor = nullptr;
		switch (MM) {
		case MarkerMode::IDLE: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_IDLE); break;
		case MarkerMode::HOVERED: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_HOVERED); break;
		case MarkerMode::SELECTED: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_SELECTED); break;
		}

		m_CorrMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID, pMarkerActor);
	}//markerModeFeatMarkerTarget
	
	void ViewportRenderManager::markerModeCorrMarkerTarget(size_t TargetPointID, MarkerMode MM) {
		if (MM == MarkerMode::DEBUG_MESH_MARKER || MM == MarkerMode::NONE) throw CForgeExcept("Invalid MarkerMode");

		CForge::StaticActor* pMarkerActor = nullptr;
		switch (MM) {
		case MarkerMode::IDLE: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE); break;
		case MarkerMode::HOVERED: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_HOVERED); break;
		case MarkerMode::SELECTED: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_SELECTED); break;
		}

		m_CorrMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID, pMarkerActor);
	}//markerModeCorrMarkerTarget

	MarkerMode ViewportRenderManager::markerModeFeatMarkerTemplate(size_t TemplatePointID) const {
		MarkerMode MM = MarkerMode::NONE;

		if (m_FeatMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID) == m_MarkerActors.at(MarkerColor::FEATURE_IDLE)) MM = MarkerMode::IDLE;
		if (m_FeatMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID) == m_MarkerActors.at(MarkerColor::FEATURE_HOVERED)) MM = MarkerMode::HOVERED;
		if (m_FeatMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID) == m_MarkerActors.at(MarkerColor::FEATURE_SELECTED)) MM = MarkerMode::SELECTED;

		return MM;
	}//markerModeFeatMarkerTemplate

	MarkerMode ViewportRenderManager::markerModeCorrMarkerTemplate(size_t TemplatePointID) const {
		MarkerMode MM = MarkerMode::NONE;

		if (m_CorrMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID) == m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE)) MM = MarkerMode::IDLE;
		if (m_CorrMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID) == m_MarkerActors.at(MarkerColor::CORRESPONDENCE_HOVERED)) MM = MarkerMode::HOVERED;
		if (m_CorrMarkerClouds.at(DatasetType::TEMPLATE).markerActor(TemplatePointID) == m_MarkerActors.at(MarkerColor::CORRESPONDENCE_SELECTED)) MM = MarkerMode::SELECTED;

		return MM;
	}//markerModeCorrMarkerTemplate

	MarkerMode ViewportRenderManager::markerModeFeatMarkerTarget(size_t TargetPointID) const {
		MarkerMode MM = MarkerMode::NONE;

		if (m_FeatMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID) == m_MarkerActors.at(MarkerColor::FEATURE_IDLE)) MM = MarkerMode::IDLE;
		if (m_FeatMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID) == m_MarkerActors.at(MarkerColor::FEATURE_HOVERED)) MM = MarkerMode::HOVERED;
		if (m_FeatMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID) == m_MarkerActors.at(MarkerColor::FEATURE_SELECTED)) MM = MarkerMode::SELECTED;

		return MM;
	}//markerModeFeatMarkerTarget

	MarkerMode ViewportRenderManager::markerModeCorrMarkerTarget(size_t TargetPointID) const {
		MarkerMode MM = MarkerMode::NONE;

		if (m_CorrMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID) == m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE)) MM = MarkerMode::IDLE;
		if (m_CorrMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID) == m_MarkerActors.at(MarkerColor::CORRESPONDENCE_HOVERED)) MM = MarkerMode::HOVERED;
		if (m_CorrMarkerClouds.at(DatasetType::TARGET).markerActor(TargetPointID) == m_MarkerActors.at(MarkerColor::CORRESPONDENCE_SELECTED)) MM = MarkerMode::SELECTED;

		return MM;
	}//markerModeCorrMarkerTarget
	
	void ViewportRenderManager::DEBUG_placeMeshMarker(DatasetType DT, Vector3f Pos) {
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_DEBUG_MeshMarkers.at(DT).translation(Pos);
	}//DEBUG_placeMeshMarker

	void ViewportRenderManager::DEBUG_showMeshMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->DEBUG_ShowDebugMeshMarker.at(DT) = Show;
	}//DEBUG_showMeshMarkerOnDataset
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// internal methods (private)
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int32_t ViewportRenderManager::addViewport(int VPPositionX, int VPPositionY, int VPSizeX, int VPSizeY, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries) {
		int32_t VPIdx = -1;
		int32_t MIdx = -1;
		for (size_t i = 0; i < m_Viewports.size(); ++i) {
			if (m_Viewports[i] == nullptr) {
				VPIdx = i;
				MIdx = i;
				break;
			}
		}
		if (VPIdx < 0) {
			VPIdx = m_Viewports.size();
			m_Viewports.push_back(nullptr);
		 }
		
		auto pView = new Viewport();

		pView->VP.Position(0) = VPPositionX;
		pView->VP.Position(1) = VPPositionY;
		pView->VP.Size(0) = VPSizeX;
		pView->VP.Size(1) = VPSizeY;

		//TODO init default orbit controls

		// init scene graph root
		pView->RootSGN.init(nullptr);
		pView->SG.init(&pView->RootSGN);
		pView->DisplayDataArrangement = DisplayDataArrangementMode::SIDE_BY_SIDE;
		
		pView->Cam.init(Vector3f(0.0f, 0.0f, 15.0f), Vector3f::UnitY());
		pView->Cam.projectionMatrix(pView->VP.Size.x(), pView->VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		for (auto it = DatasetGeometries.begin(); it != DatasetGeometries.end(); ++it)
			addDatasetDisplayData(pView, it->first, it->second.geometryType());

		for (auto& Dataset : pView->DatasetDisplayData) {
			pView->ShowDefaultHoverMarker.insert(std::pair<DatasetType, bool>(Dataset.first, false));
			pView->ShowDefaultSelectMarker.insert(std::pair<DatasetType, bool>(Dataset.first, false));
			pView->DEBUG_ShowDebugMeshMarker.insert(std::pair<DatasetType, bool>(Dataset.first, false)); //TODO: comment/remove when done with testing
			pView->ShowFeatMarkers.insert(std::pair<DatasetType, bool>(Dataset.first, false));
			pView->ShowCorrMarkers.insert(std::pair<DatasetType, bool>(Dataset.first, false));
		}

		m_Viewports[VPIdx] = pView;
		return VPIdx;
	}//addViewport

	void ViewportRenderManager::addDatasetDisplayData(Viewport* pVP, DatasetType DT, DatasetGeometryType GT) {
		if (pVP == nullptr) throw NullpointerExcept("pVP");
		if (DT == DatasetType::NONE) throw CForgeExcept("DatasetType not initialized");

		auto Inserted = pVP->DatasetDisplayData.insert(std::pair<DatasetType, DatasetDisplayData*>(DT, nullptr));
		if (!Inserted.second) throw CForgeExcept("Specified DatasetType already exists!");

		const Vector3f& Color = (DT == DatasetType::TARGET) ? m_GlobalState.DefaultTargetColor : m_GlobalState.DefaultTemplateColor;

		// match rotation of newly added dataset to existing ones
		Quaternionf Rotation = Quaternionf::Identity();
		if (pVP->DatasetDisplayData.size() > 1) Rotation = pVP->DatasetDisplayData.begin()->second->rotation();

		DatasetDisplayData* pNewDisplayData = new DatasetDisplayData();
		pNewDisplayData->init(DT, GT, &m_DatasetModels.at(DT), Color, &pVP->RootSGN, Vector3f::Zero(), Rotation, Vector3f::Ones());
		
		pVP->DatasetDisplayData.at(DT) = pNewDisplayData;
	}//addDatasetDisplayData

	void ViewportRenderManager::clearDatasetDisplayData(size_t VPIndex) {
		auto pView = m_Viewports[VPIndex];

		for (auto& DisplayData : pView->DatasetDisplayData) {
			if (DisplayData.second == nullptr) continue;
			
			DisplayData.second->removeFromSceneGraph();
			DisplayData.second->clear();
			delete DisplayData.second;
			DisplayData.second = nullptr;
		}
		pView->DatasetDisplayData.clear();
	}//clearDatasetDisplayData

	//std::vector<Vector3f> ViewportRenderManager::calculateHausdorffVertexColors(std::vector<float>& HausdorffVals) { //TODO
	//	//TODO
	//}//calculateHausdorffVertexColors

	Quaternionf ViewportRenderManager::arcballRotation(size_t VPIndex, Vector2f CursorPosStartOGL, Vector2f CursorPosEndOGL) {
		if (VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		
		Vector3f Start = mapToSphereHyperbolic(CursorPosStartOGL, m_Viewports[VPIndex]->VP.Position, m_Viewports[VPIndex]->VP.Size);
		Vector3f End = mapToSphereHyperbolic(CursorPosEndOGL, m_Viewports[VPIndex]->VP.Position, m_Viewports[VPIndex]->VP.Size);

		// rotate with camera
		Matrix3f RotMat;
		RotMat.block<3, 3>(0, 0) = m_Viewports[VPIndex]->Cam.cameraMatrix().block<3, 3>(0, 0);
		Quaternionf CamRotation = Quaternionf(RotMat.transpose());

		Start = CamRotation * Start;
		End = CamRotation * End;

		Vector3f DragDir = End - Start;

		Vector3f RotationAxis = Start.cross(End);
		RotationAxis.normalize();

		Quaternionf ModelRotation = Quaternionf::Identity();
		ModelRotation = AngleAxisf(CForge::GraphicsUtility::degToRad(DragDir.norm() * 65.0f), RotationAxis);
		return ModelRotation;
	}//arcballRotation
	
	Vector3f ViewportRenderManager::mapToSphereHyperbolic(Vector2f CursorPosOGL, Vector2i VPOffset, Vector2i VPSize) {
		float Radius = 1.0f;
		float MappedX = 2.0f * (CursorPosOGL.x() - (float)VPOffset.x()) / (float)VPSize.x() - 1.0f;
		float MappedY = 2.0f * (CursorPosOGL.y() - (float)VPOffset.y()) / (float)VPSize.y() - 1.0f;
		float MappedZ;
		float LengthSquared = (MappedX * MappedX) + (MappedY * MappedY);

		if (LengthSquared <= Radius * Radius / 2.0f) MappedZ = std::sqrtf((Radius * Radius) - LengthSquared);
		else MappedZ = ((Radius * Radius) / 2.0f) / std::sqrtf(LengthSquared);

		return Vector3f(MappedX, MappedY, MappedZ);
	}//mapToSphereHyperbolic
}