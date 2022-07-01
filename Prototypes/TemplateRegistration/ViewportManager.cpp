#include "ViewportManager.h"

#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../Examples/SceneUtilities.hpp"

namespace TempReg {
	
	ViewportManager::ViewportManager(size_t MaxViewportCount) :
		m_MaxViewportCount(MaxViewportCount), m_ActiveViewportArrangement(ViewportArrangementType::ONE_FULLSCREEN) {

		m_GlobalState.DisplayCorrespondences = false;
		m_GlobalState.GlobalMarkers = false;
		m_GlobalState.DefaultTemplateColor = Vector3f(1.0f, 0.666f, 0.498f);	// light orange
		m_GlobalState.DefaultTargetColor = Vector3f(0.541f, 0.784f, 1.0f);		// light blue
	}//Constructor

	ViewportManager::~ViewportManager() {
		for (auto& it : m_DatasetModels) it.second.clear();		
		m_DatasetModels.clear();

		// clear all viewports
		for (size_t i = 0; i < m_Viewports.size(); ++i) {
			if (m_Viewports[i] == nullptr) continue;

			clearDatasetDisplayData(i);

			m_Viewports[i]->RootSGN.clear();
			m_Viewports[i]->SG.clear();

			delete m_Viewports[i];
			m_Viewports[i] = nullptr;
			++i;
		}

		// clear all marker data
		//TODO: clear DefaultVertHoverMarkers
		//TODO: clear DefaultVertSelectMarkers

		for (auto& Root : m_FeatMarkerRoots) Root.second.removeAllChildren();
		for (auto& Root : m_CorrMarkerRoots) Root.second.removeAllChildren();
		
		for (auto it = m_FeatMarkers.begin(); it != m_FeatMarkers.end(); ++it) {
			for (size_t i = 0; i < it->second.size(); ++i) {
				it->second[i].GeomSGN.clear();
				it->second[i].TransSGN.clear();
			}
			it->second.clear();
		}
		m_FeatMarkers.clear();

		for (auto it = m_CorrMarkers.begin(); it != m_CorrMarkers.end(); ++it) {
			for (size_t i = 0; i < it->second.size(); ++i) {
				it->second[i].GeomSGN.clear();
				it->second[i].TransSGN.clear();
			}
			it->second.clear();
		}
		m_CorrMarkers.clear();

		for (auto it = m_MarkerActors.begin(); it != m_MarkerActors.end(); ++it) {
			it->second->release();
		}
		m_MarkerActors.clear();
	}//Destructor

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Viewport interactions
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<Vector4f> ViewportManager::calculateViewportTiling(ViewportArrangementType ArrangementType, Vector4f ContentArea) {
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

	void ViewportManager::loadViewports(const std::vector<Vector4f>& Tiles, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries) {
		
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

	void ViewportManager::resizeActiveViewports(const std::vector<Vector4f>& Tiles) {
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

	size_t ViewportManager::activeViewportCount(void) const {
		size_t Count = 0;
		for (size_t i = 0; i < m_Viewports.size(); ++i) {
			if (m_Viewports[i] != nullptr) ++Count;
		}
		return Count;
	}//activeViewportCount

	int32_t ViewportManager::mouseInViewport(Vector2f CursorPosOGL) const {
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
	
	void ViewportManager::updateViewport(size_t VPIndex, float FPSScale) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		
		// process local (per viewport) & global display states...
		auto pView = m_Viewports[VPIndex];
				
		// process markers 
		for (auto& Dataset : pView->DatasetDisplayData) {
			// -> link debug mesh markers to dataset display data of viewport (TODO comment/remove when done with tests!)
			if (m_DEBUG_MeshMarkers.at(Dataset.first).TransSGN.parent() != nullptr)
				m_DEBUG_MeshMarkers.at(Dataset.first).TransSGN.parent()->removeChild(&(m_DEBUG_MeshMarkers.at(Dataset.first).TransSGN));
			pView->DatasetDisplayData.at(Dataset.first)->TransSGN.addChild(&(m_DEBUG_MeshMarkers.at(Dataset.first).TransSGN));
			
			// -> link default vertex hover markers to dataset display data of viewport
			if (m_DefaultVertHoverMarkers.at(Dataset.first).TransSGN.parent() != nullptr)
				m_DefaultVertHoverMarkers.at(Dataset.first).TransSGN.parent()->removeChild(&(m_DefaultVertHoverMarkers.at(Dataset.first).TransSGN));
			pView->DatasetDisplayData.at(Dataset.first)->TransSGN.addChild(&(m_DefaultVertHoverMarkers.at(Dataset.first).TransSGN));

			// -> link default vertex selection markers to dataset display data of viewport
			if (m_DefaultVertSelectMarkers.at(Dataset.first).TransSGN.parent() != nullptr)
				m_DefaultVertSelectMarkers.at(Dataset.first).TransSGN.parent()->removeChild(&(m_DefaultVertSelectMarkers.at(Dataset.first).TransSGN));
			pView->DatasetDisplayData.at(Dataset.first)->TransSGN.addChild(&(m_DefaultVertSelectMarkers.at(Dataset.first).TransSGN));

			// -> link feature pair markers to dataset display data of viewport
			if (m_FeatMarkerRoots.at(Dataset.first).parent() != nullptr)
				m_FeatMarkerRoots.at(Dataset.first).parent()->removeChild(&(m_FeatMarkerRoots.at(Dataset.first)));
			Dataset.second->TransSGN.addChild(&(m_FeatMarkerRoots.at(Dataset.first)));

			// -> link feature pair markers to dataset display data of viewport
			if (m_CorrMarkerRoots.at(Dataset.first).parent() != nullptr)
				m_CorrMarkerRoots.at(Dataset.first).parent()->removeChild(&(m_CorrMarkerRoots.at(Dataset.first)));
			Dataset.second->TransSGN.addChild(&(m_CorrMarkerRoots.at(Dataset.first)));

			// -> decide if to show linked markers
			m_DEBUG_MeshMarkers.at(Dataset.first).TransSGN.enable(true, pView->DEBUG_ShowDebugMeshMarker.at(Dataset.first)); //TODO comment/remove when done with tests!
			m_DefaultVertHoverMarkers.at(Dataset.first).TransSGN.enable(true, pView->ShowDefaultVertexHoverMarker.at(Dataset.first));
			m_DefaultVertSelectMarkers.at(Dataset.first).TransSGN.enable(true, pView->ShowDefaultVertexSelectMarker.at(Dataset.first));
			m_FeatMarkerRoots.at(Dataset.first).enable(true, pView->ShowFeatMarkers.at(Dataset.first));
			m_CorrMarkerRoots.at(Dataset.first).enable(true, pView->ShowCorrMarkers.at(Dataset.first));
		}

		// update the scene graph for this viewport
		pView->SG.update(FPSScale);
	}//updateViewport

	void ViewportManager::renderViewport(size_t VPIndex, CForge::RenderDevice& RDev) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->SG.render(&RDev);
	}//renderViewport

	Vector2i ViewportManager::viewportPosition(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");
		return m_Viewports[VPIndex]->VP.Position;
	}//viewportPosition

	Vector2i ViewportManager::viewportSize(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");
		return m_Viewports[VPIndex]->VP.Size;
	}//viewportSize

	Vector2f ViewportManager::viewportCenter(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");

		Vector2f Pos((float)m_Viewports[VPIndex]->VP.Position.x(), (float)m_Viewports[VPIndex]->VP.Position.y());
		Vector2f Size((float)m_Viewports[VPIndex]->VP.Size.x(), (float)m_Viewports[VPIndex]->VP.Size.y());
		return (Pos + (Size / 2.0f));
	}//viewportCenter

	CForge::VirtualCamera* ViewportManager::viewportCam(size_t VPIndex) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return &m_Viewports[VPIndex]->Cam;
	}//viewportCam

	CForge::RenderDevice::Viewport ViewportManager::viewportGetRenderDeviceViewport(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");
		return m_Viewports[VPIndex]->VP;
	}//viewportGetRenderDeviceViewport
	
	void ViewportManager::viewportSetCamProjectionMode(size_t VPIndex, ViewportProjectionMode Mode) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->ProjMode = Mode;
	}//viewportSetCamProjectionMode

	void ViewportManager::viewportArcballRotate(size_t VPIndex, Vector2f Start, Vector2f End) {//TODO
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		Quaternionf Rotation = arcballRotation(VPIndex, Start, End);

		for (auto& Dataset : m_Viewports[VPIndex]->DatasetDisplayData)
			Dataset.second->TransSGN.rotation(Rotation * Dataset.second->TransSGN.rotation());
	}//viewportArcballRotate

	void ViewportManager::acrballRotateAllViewports(size_t VPIndex, Vector2f Start, Vector2f End) {//TODO
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		Quaternionf Rotation = arcballRotation(VPIndex, Start, End);

		for (auto pView : m_Viewports) {
			if (pView == nullptr) continue;
			for (auto& Dataset : pView->DatasetDisplayData)
				Dataset.second->TransSGN.rotation(Rotation * Dataset.second->TransSGN.rotation());
		}
	}//acrballRotateAllViewports

	void ViewportManager::viewportSetCamPos(size_t VPIndex, Vector3f Eye) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitEye = Eye;
	}//viewportSetCamPos

	void ViewportManager::viewportSetCamTarget(size_t VPIndex, Vector3f Target) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitTarget = Target;
	}//viewportSetCamTarget

	void ViewportManager::viewportSetCamZoom(size_t VPIndex, float Zoom) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitZoom = Zoom;
	}//viewportSetCamZoom

	void ViewportManager::viewportMoveCamPos(size_t VPIndex, Vector3f Movement) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitEye += Movement;
	}//viewportMoveCamPos

	void ViewportManager::viewportMoveCamTarget(size_t VPIndex, Vector3f Movement) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitTarget += Movement;
	}//viewportMoveCamTarget

	void ViewportManager::viewportZoomCam(size_t VPIndex, float Change) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitZoom += Change;
	}//viewportZoomCam

	ViewportProjectionMode ViewportManager::viewportCamProjectionMode(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->ProjMode;
	}//viewportCamProjectionMode

	Matrix4f ViewportManager::viewportViewMatrix(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->Cam.cameraMatrix();
	}//viewportViewMatrix

	Matrix4f ViewportManager::viewportProjectionMatrix(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->Cam.projectionMatrix();
	}//viewportProjectionMatrix

	Vector3f ViewportManager::viewportCamPos(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->OrbitEye;
	}//viewportCamPos

	Vector3f ViewportManager::viewportCamTarget(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->OrbitTarget;
	}//viewportCamTarget

	float ViewportManager::viewportCamZoom(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->OrbitZoom;
	}//viewportCamZoom

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Dataset interactions
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ViewportManager::initDatasetModelFromFile(DatasetType DT, std::string Filepath) {
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

	void ViewportManager::updateDatasetModel(DatasetType DT/*, new vertices here...*/) { //TODO
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

	void ViewportManager::updateDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData>::iterator itDatasetGeom) { //TODO
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		
		auto pView = m_Viewports[VPIndex];

		//TODO...

	}//updateDatasetDisplayData

	void ViewportManager::showDatasetDisplayData(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->DatasetDisplayData.at(DT)->TransSGN.enable(true, Show);
	}//showDatasetDisplayData

	void ViewportManager::enableWireframeActor(size_t VPIndex, DatasetType DT, bool Active) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->DatasetDisplayData.at(DT)->WireframeGeomSGN.enable(true, Active);
	}//enableWireframeActor

	void ViewportManager::enablePrimitivesActor(size_t VPIndex, DatasetType DT, bool Active) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->DatasetDisplayData.at(DT)->PrimitivesGeomSGN.enable(true, Active);
	}//enablePrimitivesActor

	void ViewportManager::setSolidColorShading(size_t VPIndex, DatasetType DT, bool PrimitivesActor, Vector3f* Color) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto pView = m_Viewports[VPIndex];
		std::vector<Vector3f> Colors;

		switch (DT) {
		case DatasetType::TEMPLATE:
		case DatasetType::DTEMPLATE: {
			if (Color != nullptr) for (uint32_t i = 0; i < m_DatasetModels.at(DT).vertexCount(); ++i) Colors.push_back(*Color);
			else for (uint32_t i = 0; i < m_DatasetModels.at(DT).vertexCount(); ++i) Colors.push_back(m_GlobalState.DefaultTemplateColor);
			break;
		}
		case DatasetType::TARGET: {
			if (Color != nullptr) for (uint32_t i = 0; i < m_DatasetModels.at(DT).vertexCount(); ++i) Colors.push_back(*Color);
			else for (uint32_t i = 0; i < m_DatasetModels.at(DT).vertexCount(); ++i) Colors.push_back(m_GlobalState.DefaultTargetColor); 
			break;
		}
		case DatasetType::NONE: throw CForgeExcept("DatasetType not initialized"); break;
		default: break;
		}

		m_DatasetModels.at(DT).colors(&Colors);
		Colors.clear();

		// update DatasetActor specified by PrimitivesActor boolean
		auto pDisplayData = pView->DatasetDisplayData.at(DT);
		DatasetRenderMode RM;
		
		if (PrimitivesActor) {
			RM = pView->DatasetDisplayData.at(DT)->pPrimitivesActor->renderMode();
			//DisplayData->TransSGN.removeChild(&(DisplayData->PrimitivesGeomSGN));
			//DisplayData->PrimitivesGeomSGN.clear();
			pDisplayData->pPrimitivesActor->release();
			pDisplayData->pPrimitivesActor = new DatasetActor();
			pDisplayData->pPrimitivesActor->init(&m_DatasetModels.at(DT), RM);
			//DisplayData->PrimitivesGeomSGN.init(&(DisplayData->TransSGN), DisplayData->pPrimitivesActor);
			pDisplayData->PrimitivesGeomSGN.actor(pDisplayData->pPrimitivesActor);
		}
		else {
			RM = pView->DatasetDisplayData.at(DT)->pWireframeActor->renderMode();
			//DisplayData->TransSGN.removeChild(&(DisplayData->WireframeGeomSGN));
			//DisplayData->WireframeGeomSGN.clear();
			pDisplayData->pWireframeActor->release();
			pDisplayData->pWireframeActor = new DatasetActor();
			pDisplayData->pWireframeActor->init(&m_DatasetModels.at(DT), RM);
			//DisplayData->WireframeGeomSGN.init(&(DisplayData->TransSGN), DisplayData->pWireframeActor);
			pDisplayData->WireframeGeomSGN.actor(pDisplayData->pWireframeActor);
		}		
	}//setSolidColorShading
	
	//TODO: change to support pPrimitivesActor and pWireframeActor!
	//void ViewportManager::setHausdorffDistColorShading(size_t VPIndex, DatasetType DT, std::vector<float>& HausdorffVals) {
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

	void ViewportManager::setDatasetDisplayDataArrangement(size_t VPIndex, DisplayDataArrangementMode Arrangement) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto pView = m_Viewports[VPIndex];
		pView->DisplayDataArrangement = Arrangement;
		if (Arrangement == DisplayDataArrangementMode::LAYERED) arrangeDatasetDisplayDataLayered(VPIndex);
		else arrangeDatasetDisplayDataSideBySide(VPIndex);
	}//setDatasetDisplayDataArrangement

	void ViewportManager::arrangeDatasetDisplayDataLayered(size_t VPIndex) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto pView = m_Viewports[VPIndex];

		for (auto it = pView->DatasetDisplayData.begin(); it != pView->DatasetDisplayData.end(); ++it)
			it->second->TransSGN.translation(Vector3f::Zero());
		
		pView->DisplayDataArrangement = DisplayDataArrangementMode::LAYERED;
	}//arrangeDatasetDisplayDataLayered

	void ViewportManager::arrangeDatasetDisplayDataSideBySide(size_t VPIndex) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto pView = m_Viewports[VPIndex];

		// update positions of all existing datasets	
		bool TemplateShown = false;
		bool DTemplateShown = false;
		bool TargetShown = false;
		float XWidthTotal = 0.0f;
		float MaxExtentTemplate = -1.0f;
		float MaxExtentDTemplate = -1.0f;
		float MaxExtentTarget = -1.0f;
		
		// -> sum up biggest extents of bounding box of each displayed dataset
		auto pDisplayData = pView->DatasetDisplayData.at(DatasetType::TEMPLATE);
		pDisplayData->TransSGN.enabled(nullptr, &TemplateShown);
		if (TemplateShown) {
			auto& Model = m_DatasetModels.at(DatasetType::TEMPLATE);
			Vector3f Extents = (Model.aabb().Max - Model.aabb().Min).cwiseAbs();
			MaxExtentTemplate = Extents(0);
			if (Extents(1) > MaxExtentTemplate) MaxExtentTemplate = Extents(1);
			if (Extents(2) > MaxExtentTemplate) MaxExtentTemplate = Extents(2);
			XWidthTotal += MaxExtentTemplate;
		}
		
		pDisplayData = pView->DatasetDisplayData.at(DatasetType::DTEMPLATE);
		pDisplayData->TransSGN.enabled(nullptr, &DTemplateShown);
		if (DTemplateShown) {
			auto& Model = m_DatasetModels.at(DatasetType::DTEMPLATE);
			Vector3f Extents = (Model.aabb().Max - Model.aabb().Min).cwiseAbs();
			MaxExtentDTemplate = Extents(0);
			if (Extents(1) > MaxExtentDTemplate) MaxExtentDTemplate = Extents(1);
			if (Extents(2) > MaxExtentDTemplate) MaxExtentDTemplate = Extents(2);
			XWidthTotal += MaxExtentDTemplate;
		}
		
		pDisplayData = pView->DatasetDisplayData.at(DatasetType::TARGET);
		pDisplayData->TransSGN.enabled(nullptr, &TargetShown);
		if (TargetShown) {
			auto& Model = m_DatasetModels.at(DatasetType::TARGET);
			Vector3f Extents = (Model.aabb().Max - Model.aabb().Min).cwiseAbs();
			MaxExtentTarget = Extents(0);
			if (Extents(1) > MaxExtentTarget) MaxExtentTarget = Extents(1);
			if (Extents(2) > MaxExtentTarget) MaxExtentTarget = Extents(2);
			XWidthTotal += MaxExtentTarget;
		}
		
		// -> center arrangement on world space origin
		float XStart = -(XWidthTotal * 0.5f);
				
		if (TemplateShown) {
			auto BB = m_DatasetModels.at(DatasetType::TEMPLATE).aabb();
			Vector3f Position = Vector3f(XStart + (MaxExtentTemplate * 0.5f), 0.0f, 0.0f);
			pView->DatasetDisplayData.at(DatasetType::TEMPLATE)->TransSGN.translation(Position);
			XStart += MaxExtentTemplate;
		}
				
		if (DTemplateShown) {
			auto BB = m_DatasetModels.at(DatasetType::DTEMPLATE).aabb();
			Vector3f Position = Vector3f(XStart + (MaxExtentDTemplate * 0.5f), 0.0f, 0.0f);
			pView->DatasetDisplayData.at(DatasetType::DTEMPLATE)->TransSGN.translation(Position);
			XStart += MaxExtentDTemplate;
		}
				
		if (TargetShown) {
			auto BB = m_DatasetModels.at(DatasetType::TARGET).aabb();
			Vector3f Position = Vector3f(XStart + (MaxExtentTarget * 0.5f), 0.0f, 0.0f);
			pView->DatasetDisplayData.at(DatasetType::TARGET)->TransSGN.translation(Position);
		}

		pView->DisplayDataArrangement = DisplayDataArrangementMode::SIDE_BY_SIDE;
	}//arrangeDatasetDispalyDataSideBySide

	DisplayDataArrangementMode ViewportManager::activeDatasetDisplayDataArrangement(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->DisplayDataArrangement;
	}//activeDatasetDisplayDataArrangement

	std::vector<DatasetType> ViewportManager::activeDatasetTypes(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto pView = m_Viewports[VPIndex];
		std::vector<DatasetType> ActiveTypes;

		for (auto it = pView->DatasetDisplayData.begin(); it != pView->DatasetDisplayData.end(); ++it) {
			bool DatasetShown;
			it->second->TransSGN.enabled(nullptr, &DatasetShown);
			if (DatasetShown) ActiveTypes.push_back(it->first);
		}

		return ActiveTypes;
	}//activeDatasetTypes

	Matrix4f ViewportManager::datasetModelMatrix(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		
		auto pView = m_Viewports[VPIndex];
		if (pView->DatasetDisplayData.count(DT) == 0) {
			std::string Type;
			if (DT == DatasetType::TEMPLATE) Type = "TEMPLATE";
			if (DT == DatasetType::DTEMPLATE) Type = "DTEMPLATE";
			if (DT == DatasetType::TARGET) Type = "TARGET";
			throw CForgeExcept("Dataset " + Type + " not active");
		}

		Vector3f DatasetTranslation = Vector3f::Zero();
		Quaternionf DatasetRotation = Quaternionf::Identity();
		Vector3f DatasetScaling = Vector3f::Ones();
		pView->DatasetDisplayData.at(DT)->TransSGN.buildTansformation(&DatasetTranslation, &DatasetRotation, &DatasetScaling);

		Matrix4f TransMat = Matrix4f::Identity();
		TransMat(0, 3) = DatasetTranslation(0);
		TransMat(1, 3) = DatasetTranslation(1);
		TransMat(2, 3) = DatasetTranslation(2);

		Matrix4f RotMat = Matrix4f::Identity();
		Matrix3f RotQuatToMat = DatasetRotation.toRotationMatrix();
		RotMat.block<3, 3>(0, 0) = RotQuatToMat;

		Matrix4f ScaleMat = Matrix4f::Identity();
		ScaleMat(0, 0) = DatasetScaling(0);
		ScaleMat(1, 1) = DatasetScaling(1);
		ScaleMat(2, 2) = DatasetScaling(2);

		Matrix4f Model = TransMat * RotMat * ScaleMat;
		return Model;
	}//datasetModelMatrix

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Vertex marker interactions
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ViewportManager::initMarkerData(std::string Filepath) {
		CForge::T3DMesh<float> RawMesh;
		CForge::SAssetIO::load(Filepath, &RawMesh);
		CForge::SceneUtilities::setMeshShader(&RawMesh, 1.0f, 0.0f);
		RawMesh.computePerVertexNormals();

		Vector3f Color;
		std::vector<Vector3f> Colors;

		// Initialize marker model for MarkerColor::DEFAULT_VERTEX_HOVERING
		Color = Vector3f(1.0f, 1.0f, 1.0f); // white #ffffff
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors.push_back(Color);
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::DEFAULT_VERTEX_HOVERING, nullptr));
		CForge::StaticActor* pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::DEFAULT_VERTEX_HOVERING) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::DEFAULT_VERTEX_SELECTION
		Color = Vector3f(1.0f, 0.956f, 0.478f); // light yellow #fff47a
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::DEFAULT_VERTEX_SELECTION, nullptr));
		pNewMarkerActor = new CForge::StaticActor(); 
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::DEFAULT_VERTEX_SELECTION) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::FEATURE_IDLE
		Color = Vector3f(0.137f, 0.341f, 0.843f); // blue #2357d7
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::FEATURE_IDLE, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::FEATURE_IDLE) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::FEATURE_HOVERED
		Color = Vector3f(0.643f, 0.733f, 0.956f); // light blue #a4bbf4
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::FEATURE_HOVERED, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::FEATURE_HOVERED) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::FEATURE_SELECTED
		Color = Vector3f(0.078f, 1.0f, 0.960f); // bright turquoise #14fff5
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::FEATURE_SELECTED, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::FEATURE_SELECTED) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::CORRESPONDENCE_IDLE
		Color = Vector3f(0.980f, 0.772f, 0.0f); // orange #fac500
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::CORRESPONDENCE_IDLE, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::CORRESPONDENCE_HOVERED
		Color = Vector3f(1.0f, 0.949f, 0.760f); // light orange #fff2c2
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = Color;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerColor, CForge::StaticActor*>(MarkerColor::CORRESPONDENCE_HOVERED, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerColor::CORRESPONDENCE_HOVERED) = pNewMarkerActor;

		// Initialize marker model for MarkerColor::CORRESPONDENCE_SELECTED
		Color = Vector3f(1.0f, 0.956f, 0.478f); // light yellow #fff47a
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

		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::DEFAULT_VERTEX_HOVERING);
		m_DefaultVertHoverMarkers.try_emplace(DatasetType::TEMPLATE);
		m_DefaultVertHoverMarkers.try_emplace(DatasetType::DTEMPLATE);
		m_DefaultVertHoverMarkers.try_emplace(DatasetType::TARGET);
		for (auto& M : m_DefaultVertHoverMarkers) {
			M.second.TransSGN.init(nullptr);
			M.second.TransSGN.scale(Vector3f(0.05f, 0.05f, 0.05f));
			M.second.GeomSGN.init(&(M.second.TransSGN), pMarkerActor);
		}

		pMarkerActor = m_MarkerActors.at(MarkerColor::DEFAULT_VERTEX_SELECTION);
		m_DefaultVertSelectMarkers.try_emplace(DatasetType::TEMPLATE);
		m_DefaultVertSelectMarkers.try_emplace(DatasetType::DTEMPLATE);
		m_DefaultVertSelectMarkers.try_emplace(DatasetType::TARGET);
		for (auto& M : m_DefaultVertSelectMarkers) {
			M.second.TransSGN.init(nullptr);
			M.second.TransSGN.scale(Vector3f(0.05f, 0.05f, 0.05f));
			M.second.GeomSGN.init(&(M.second.TransSGN), pMarkerActor);
		}

		pMarkerActor = m_MarkerActors.at(MarkerColor::DEBUG);
		m_DEBUG_MeshMarkers.try_emplace(DatasetType::TEMPLATE);
		m_DEBUG_MeshMarkers.try_emplace(DatasetType::DTEMPLATE);
		m_DEBUG_MeshMarkers.try_emplace(DatasetType::TARGET);
		for (auto& M : m_DEBUG_MeshMarkers) {
			M.second.TransSGN.init(nullptr);
			M.second.TransSGN.scale(Vector3f(0.05f, 0.05f, 0.05f));
			M.second.GeomSGN.init(&(M.second.TransSGN), pMarkerActor);
		}
		
		m_FeatMarkerRoots.insert(std::pair<DatasetType, CForge::SGNTransformation>(DatasetType::TEMPLATE, CForge::SGNTransformation()));
		m_FeatMarkerRoots.insert(std::pair<DatasetType, CForge::SGNTransformation>(DatasetType::DTEMPLATE, CForge::SGNTransformation()));
		m_FeatMarkerRoots.insert(std::pair<DatasetType, CForge::SGNTransformation>(DatasetType::TARGET, CForge::SGNTransformation()));
		for (auto& Root : m_FeatMarkerRoots) Root.second.init(nullptr);

		m_CorrMarkerRoots.insert(std::pair<DatasetType, CForge::SGNTransformation>(DatasetType::TEMPLATE, CForge::SGNTransformation()));
		m_CorrMarkerRoots.insert(std::pair<DatasetType, CForge::SGNTransformation>(DatasetType::DTEMPLATE, CForge::SGNTransformation()));
		m_CorrMarkerRoots.insert(std::pair<DatasetType, CForge::SGNTransformation>(DatasetType::TARGET, CForge::SGNTransformation()));
		for (auto& Root : m_CorrMarkerRoots) Root.second.init(nullptr);

		for (auto& M : m_FeatMarkers) M.second.reserve(256);
		for (auto& M : m_CorrMarkers) M.second.reserve(256);
	}//initMarkerData

	void ViewportManager::placeDefaultVertexHoverMarker(size_t VPIndex, DatasetType DT, const Vector3f VertexPos) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_DefaultVertHoverMarkers.at(DT).TransSGN.translation(VertexPos); // move marker to current vertex position
	}//placeDefaultVertexHoverMarker

	void ViewportManager::placeDefaultVertSelectMarker(size_t VPIndex, DatasetType DT, const Vector3f VertexPos) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_DefaultVertSelectMarkers.at(DT).TransSGN.translation(VertexPos); // move marker to current vertex position
	}//placeDefaultVertSelectMarker

	bool ViewportManager::showDefaultVertexHoverMarkerOnDataset(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		return m_Viewports[VPIndex]->ShowDefaultVertexHoverMarker.at(DT);
	}//showDefaultVertexHoverMarkerOnDataset

	void ViewportManager::showDefaultVertexHoverMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->ShowDefaultVertexHoverMarker.at(DT) = Show;
	}//showDefaultVertexHoverMarkerOnDataset

	bool ViewportManager::showDefaultVertSelectMarkerOnDataset(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		return m_Viewports[VPIndex]->ShowDefaultVertexSelectMarker.at(DT);
	}//showDefaultVertSelectMarkerOnDataset

	void ViewportManager::showDefaultVertSelectMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->ShowDefaultVertexSelectMarker.at(DT) = Show;
	}//showDefaultVertSelectMarkerOnDataset

	void ViewportManager::addFeatMarkerTemplate(size_t FeatPointID, const Vector3f MarkerPos) {
		if (m_TemplateFeatIDLookup.count(FeatPointID) > 0) return; // marker already exists, nothing to do

		// Place markers on template and deformed template
		int64_t InternalID = -1;
		if (!m_FreeFeatMarkersTemplate.empty()) {
			InternalID = *m_FreeFeatMarkersTemplate.begin();
			m_FreeFeatMarkersTemplate.erase(m_FreeFeatMarkersTemplate.begin());
		}
		else {
			InternalID = m_FeatMarkers.at(DatasetType::TEMPLATE).size();
			m_FeatMarkers.at(DatasetType::TEMPLATE).push_back(MarkerSGData());
		}

		// create scene graph data for template and deformed template
		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_IDLE);

		m_FeatMarkers.at(DatasetType::TEMPLATE)[InternalID].TransSGN.init(&(m_FeatMarkerRoots.at(DatasetType::TEMPLATE)), MarkerPos);
		m_FeatMarkers.at(DatasetType::DTEMPLATE)[InternalID].TransSGN.init(&(m_FeatMarkerRoots.at(DatasetType::DTEMPLATE)), MarkerPos);
		m_FeatMarkers.at(DatasetType::TEMPLATE)[InternalID].GeomSGN.init(&(m_FeatMarkers.at(DatasetType::TEMPLATE)[InternalID].TransSGN), pMarkerActor);
		m_FeatMarkers.at(DatasetType::DTEMPLATE)[InternalID].GeomSGN.init(&(m_FeatMarkers.at(DatasetType::DTEMPLATE)[InternalID].TransSGN), pMarkerActor);

		m_TemplateFeatIDLookup.insert(std::pair<size_t, size_t>(FeatPointID, InternalID));
	}//addFeatMarkerTemplate
	
	void ViewportManager::addCorrMarkerTemplate(size_t CorrPointID, const Vector3f MarkerPos) {
		if (m_TemplateCorrIDLookup.count(CorrPointID) > 0) return; // marker already exists, nothing to do

		// Place markers on template and deformed template
		int64_t InternalID = -1;
		if (!m_FreeCorrMarkersTemplate.empty()) {
			InternalID = *m_FreeCorrMarkersTemplate.begin();
			m_FreeCorrMarkersTemplate.erase(m_FreeCorrMarkersTemplate.begin());
		}
		else {
			InternalID = m_CorrMarkers.at(DatasetType::TEMPLATE).size();
			m_CorrMarkers.at(DatasetType::TEMPLATE).push_back(MarkerSGData());
		}

		// create scene graph data for template and deformed template
		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE);

		m_CorrMarkers.at(DatasetType::TEMPLATE)[InternalID].TransSGN.init(&(m_CorrMarkerRoots.at(DatasetType::TEMPLATE)), MarkerPos);
		m_CorrMarkers.at(DatasetType::DTEMPLATE)[InternalID].TransSGN.init(&(m_CorrMarkerRoots.at(DatasetType::DTEMPLATE)), MarkerPos);
		m_CorrMarkers.at(DatasetType::TEMPLATE)[InternalID].GeomSGN.init(&(m_CorrMarkers.at(DatasetType::TEMPLATE)[InternalID].TransSGN), pMarkerActor);
		m_CorrMarkers.at(DatasetType::DTEMPLATE)[InternalID].GeomSGN.init(&(m_CorrMarkers.at(DatasetType::DTEMPLATE)[InternalID].TransSGN), pMarkerActor);

		m_TemplateCorrIDLookup.insert(std::pair<size_t, size_t>(CorrPointID, InternalID));
	}//addCorrMarkerTemplate

	void ViewportManager::addFeatMarkerTarget(size_t FeatPointID, const Vector3f MarkerPos) {
		// Place marker on target if necessary
		// m_TargetFeatIDLookup.count(TargetPointID) > 0 if a marker of this type already exists at given position 
		// (i.e. a previously added pair of feature points added a marker on the target at the same position, adding another marker would mean 
		// unnecessary overlap; TargetMarkerIDs are computed externally)
		if (m_TargetFeatIDLookup.count(FeatPointID) > 0) return;

		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_IDLE);
		int64_t InternalID = -1;
		if (!m_FreeFeatMarkersTarget.empty()) {
			InternalID = *m_FreeFeatMarkersTarget.begin();
			m_FreeFeatMarkersTarget.erase(m_FreeFeatMarkersTarget.begin());
		}
		else {
			InternalID = m_FeatMarkers.at(DatasetType::TARGET).size();
			m_FeatMarkers.at(DatasetType::TARGET).push_back(MarkerSGData());
		}

		// create scene graph data for target
		m_FeatMarkers.at(DatasetType::TARGET)[InternalID].TransSGN.init(&(m_FeatMarkerRoots.at(DatasetType::TARGET)), MarkerPos);
		m_FeatMarkers.at(DatasetType::TARGET)[InternalID].GeomSGN.init(&(m_FeatMarkers.at(DatasetType::TARGET)[InternalID].TransSGN), pMarkerActor);

		m_TargetFeatIDLookup.insert(std::pair<size_t, size_t>(FeatPointID, InternalID));
	}//addFeatMarkerTarget
		
	void ViewportManager::addCorrMarkerTarget(size_t CorrPointID, const Vector3f MarkerPos) {
		// Place marker on target if necessary
		// m_TargetCorrIDLookup.count(TargetPointID) > 0 if a marker of this type already exists at given position 
		// (i.e. a previously added pair of correspondence points added a marker on the target at the same position, adding another marker would mean 
		// unnecessary overlap; TargetMarkerIDs are computed externally)
		if (m_TargetCorrIDLookup.count(CorrPointID) > 0) return;
		
		CForge::StaticActor* pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE);
		int64_t InternalID = -1;
		if (!m_FreeCorrMarkersTarget.empty()) {
			InternalID = *m_FreeCorrMarkersTarget.begin();
			m_FreeCorrMarkersTarget.erase(m_FreeCorrMarkersTarget.begin());
		}
		else {
			InternalID = m_CorrMarkers.at(DatasetType::TARGET).size();
			m_CorrMarkers.at(DatasetType::TARGET).push_back(MarkerSGData());
		}

		// create scene graph data for target
		m_CorrMarkers.at(DatasetType::TARGET)[InternalID].TransSGN.init(&(m_CorrMarkerRoots.at(DatasetType::TARGET)), MarkerPos);
		m_CorrMarkers.at(DatasetType::TARGET)[InternalID].GeomSGN.init(&(m_CorrMarkers.at(DatasetType::TARGET)[InternalID].TransSGN), pMarkerActor);

		m_TargetCorrIDLookup.insert(std::pair<size_t, size_t>(CorrPointID, InternalID));
	}//addCorrMarkerTarget

	void ViewportManager::removeFeatMarkerTemplate(size_t FeatPointID) {
		auto InternalID = m_TemplateFeatIDLookup.find(FeatPointID);
		if (InternalID == m_TemplateFeatIDLookup.end()) return;
		
		m_FeatMarkerRoots.at(DatasetType::TEMPLATE).removeChild(&(m_FeatMarkers.at(DatasetType::TEMPLATE)[InternalID->second].TransSGN));
		m_FeatMarkerRoots.at(DatasetType::DTEMPLATE).removeChild(&(m_FeatMarkers.at(DatasetType::DTEMPLATE)[InternalID->second].TransSGN));
		
		m_FeatMarkers.at(DatasetType::TEMPLATE)[InternalID->second].GeomSGN.clear();
		m_FeatMarkers.at(DatasetType::TEMPLATE)[InternalID->second].TransSGN.clear();
		m_FeatMarkers.at(DatasetType::DTEMPLATE)[InternalID->second].GeomSGN.clear();
		m_FeatMarkers.at(DatasetType::DTEMPLATE)[InternalID->second].TransSGN.clear();

		m_FreeFeatMarkersTemplate.insert(InternalID->second);
		m_TemplateFeatIDLookup.erase(InternalID);
	}//removeFeatMarkerTemplate

	void ViewportManager::removeCorrMarkerTemplate(size_t CorrPointID) {
		auto InternalID = m_TemplateCorrIDLookup.find(CorrPointID);
		if (InternalID == m_TemplateCorrIDLookup.end()) return;
		
		m_CorrMarkerRoots.at(DatasetType::TEMPLATE).removeChild(&(m_CorrMarkers.at(DatasetType::TEMPLATE)[InternalID->second].TransSGN));
		m_CorrMarkerRoots.at(DatasetType::DTEMPLATE).removeChild(&(m_CorrMarkers.at(DatasetType::DTEMPLATE)[InternalID->second].TransSGN));
		
		m_CorrMarkers.at(DatasetType::TEMPLATE)[InternalID->second].GeomSGN.clear();
		m_CorrMarkers.at(DatasetType::TEMPLATE)[InternalID->second].TransSGN.clear();
		m_CorrMarkers.at(DatasetType::DTEMPLATE)[InternalID->second].GeomSGN.clear();
		m_CorrMarkers.at(DatasetType::DTEMPLATE)[InternalID->second].TransSGN.clear();

		m_FreeCorrMarkersTemplate.insert(InternalID->second);
		m_TemplateCorrIDLookup.erase(InternalID);
	}//removeCorrMarkerTemplate

	void ViewportManager::removeFeatMarkerTarget(size_t FeatPointID) {
		auto InternalID = m_TargetFeatIDLookup.find(FeatPointID);
		if (InternalID == m_TargetFeatIDLookup.end()) return;

		m_FeatMarkerRoots.at(DatasetType::TARGET).removeChild(&(m_FeatMarkers.at(DatasetType::TARGET)[InternalID->second].TransSGN));

		m_FeatMarkers.at(DatasetType::TARGET)[InternalID->second].GeomSGN.clear();
		m_FeatMarkers.at(DatasetType::TARGET)[InternalID->second].TransSGN.clear();

		m_FreeFeatMarkersTarget.insert(InternalID->second);
		m_TargetFeatIDLookup.erase(InternalID);
	}//removeFeatMarkerTarget

	void ViewportManager::removeCorrMarkerTarget(size_t CorrPointID) {
		auto InternalID = m_TargetCorrIDLookup.find(CorrPointID);
		if (InternalID == m_TargetCorrIDLookup.end()) return;

		m_CorrMarkerRoots.at(DatasetType::TARGET).removeChild(&(m_CorrMarkers.at(DatasetType::TARGET)[InternalID->second].TransSGN));

		m_CorrMarkers.at(DatasetType::TARGET)[InternalID->second].GeomSGN.clear();
		m_CorrMarkers.at(DatasetType::TARGET)[InternalID->second].TransSGN.clear();

		m_FreeCorrMarkersTarget.insert(InternalID->second);
		m_TargetCorrIDLookup.erase(InternalID);
	}//removeCorrMarkerTarget

	void ViewportManager::clearFeatMarkers(void) {
		m_FeatMarkerRoots.at(DatasetType::TEMPLATE).removeAllChildren();
		m_FeatMarkerRoots.at(DatasetType::DTEMPLATE).removeAllChildren();
		m_FeatMarkerRoots.at(DatasetType::TARGET).removeAllChildren();

		for (auto& M : m_FeatMarkers) {
			for (auto& Marker : M.second) {
				Marker.GeomSGN.clear();
				Marker.TransSGN.clear();
			}			
			M.second.clear();
		}

		m_FreeFeatMarkersTemplate.clear();
		m_FreeFeatMarkersTarget.clear();

		m_TemplateFeatIDLookup.clear();
		m_TargetFeatIDLookup.clear();
	}//clearFeatMarkers

	void ViewportManager::clearCorrMarkers(void) {
		m_CorrMarkerRoots.at(DatasetType::TEMPLATE).removeAllChildren();
		m_CorrMarkerRoots.at(DatasetType::DTEMPLATE).removeAllChildren();
		m_CorrMarkerRoots.at(DatasetType::TARGET).removeAllChildren();

		for (auto& M : m_CorrMarkers) {
			for (auto& Marker : M.second) {
				Marker.GeomSGN.clear();
				Marker.TransSGN.clear();
			}
			M.second.clear();
		}

		m_FreeCorrMarkersTemplate.clear();
		m_FreeCorrMarkersTarget.clear();

		m_TemplateCorrIDLookup.clear();
		m_TargetCorrIDLookup.clear();
	}//clearCorrMarkers

	void ViewportManager::updateFeatMarkerPosition(DatasetType DT, size_t FeatPointID, const Vector3f MarkerPos) {
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType defined");

		const auto& CorrIDLookup = (DT == DatasetType::TARGET) ? m_TargetFeatIDLookup : m_TemplateFeatIDLookup;

		auto MarkerID = CorrIDLookup.find(FeatPointID);
		if (MarkerID == CorrIDLookup.end()) throw CForgeExcept("Could not find marker for specified FeatPointID");
		
		m_FeatMarkers.at(DT)[MarkerID->second].TransSGN.translation(MarkerPos);
	}//updateFeatMarkerPosition
	
	void ViewportManager::updateCorrMarkerPosition(DatasetType DT, size_t CorrPointID, const Vector3f MarkerPos) {
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType defined");
				
		const auto& CorrIDLookup = (DT == DatasetType::TARGET) ? m_TargetCorrIDLookup : m_TemplateCorrIDLookup;
		
		auto MarkerID = CorrIDLookup.find(CorrPointID);
		if (MarkerID == CorrIDLookup.end()) throw CForgeExcept("Could not find marker for specified CorrPointID");

		m_CorrMarkers.at(DT)[MarkerID->second].TransSGN.translation(MarkerPos);
	}//updateCorrMarkerPosition

	bool ViewportManager::showFeatMarkersOnDataset(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		return m_Viewports[VPIndex]->ShowFeatMarkers.at(DT);
	}//showFeatMarkersOnDataset
	
	void ViewportManager::showFeatMarkersOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->ShowFeatMarkers.at(DT) = Show;
	}//showFeatMarkersOnDataset
	
	bool ViewportManager::showCorrMarkersOnDataset(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		return m_Viewports[VPIndex]->ShowCorrMarkers.at(DT);
	}//showCorrMarkersOnDataset
	
	void ViewportManager::showCorrMarkersOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->ShowCorrMarkers.at(DT) = Show;
	}//showCorrMarkersOnDataset

	void ViewportManager::markerModeFeatMarkerPair(DatasetType DT, size_t TemplatePointID, size_t TargetPointID, MarkerMode MM) {
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		if (MM == MarkerMode::DEBUG_MESH_MARKER || MM == MarkerMode::NONE) throw CForgeExcept("Invalid MarkerMode");
		if (m_TemplateFeatIDLookup.count(TemplatePointID) == 0) throw CForgeExcept("TemplatePointID does not exist");
		if (m_TargetFeatIDLookup.count(TargetPointID) == 0) throw CForgeExcept("TargetPointID does not exist");

		size_t TemplateInternalID = m_TemplateFeatIDLookup.at(TemplatePointID);
		size_t TargetInternalID = m_TargetFeatIDLookup.at(TargetPointID);

		CForge::StaticActor* pMarkerActor = nullptr;
		switch (MM) {
		case MarkerMode::IDLE: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_IDLE); break;
		case MarkerMode::HOVERED: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_HOVERED); break;
		case MarkerMode::SELECTED: pMarkerActor = m_MarkerActors.at(MarkerColor::FEATURE_SELECTED); break;
		}
		
		m_FeatMarkers.at(DatasetType::TEMPLATE)[TemplateInternalID].GeomSGN.actor(pMarkerActor);
		m_FeatMarkers.at(DatasetType::DTEMPLATE)[TemplateInternalID].GeomSGN.actor(pMarkerActor);
		m_FeatMarkers.at(DatasetType::TARGET)[TargetInternalID].GeomSGN.actor(pMarkerActor);
	}//markerModeFeatMarkerPair

	void ViewportManager::markerModeCorrMarkerPair(DatasetType DT, size_t TemplatePointID, size_t TargetPointID, MarkerMode MM) {
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		if (MM == MarkerMode::DEBUG_MESH_MARKER || MM == MarkerMode::NONE) throw CForgeExcept("Invalid MarkerMode");
		if (m_TemplateCorrIDLookup.count(TemplatePointID) == 0) throw CForgeExcept("TemplatePointID does not exist");
		if (m_TargetCorrIDLookup.count(TargetPointID) == 0) throw CForgeExcept("TargetPointID does not exist");

		size_t TemplateInternalID = m_TemplateCorrIDLookup.at(TemplatePointID);
		size_t TargetInternalID = m_TargetCorrIDLookup.at(TargetPointID);

		CForge::StaticActor* pMarkerActor = nullptr;
		switch (MM) {
		case MarkerMode::IDLE: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_IDLE); break;
		case MarkerMode::HOVERED: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_HOVERED); break;
		case MarkerMode::SELECTED: pMarkerActor = m_MarkerActors.at(MarkerColor::CORRESPONDENCE_SELECTED); break;
		}

		m_CorrMarkers.at(DatasetType::TEMPLATE)[TemplateInternalID].GeomSGN.actor(pMarkerActor);
		m_CorrMarkers.at(DatasetType::DTEMPLATE)[TemplateInternalID].GeomSGN.actor(pMarkerActor);
		m_CorrMarkers.at(DatasetType::TARGET)[TargetInternalID].GeomSGN.actor(pMarkerActor);
	}//markerModeCorrMarkerPair
	
	void ViewportManager::DEBUG_placeMeshMarker(size_t VPIndex, DatasetType DT, Vector3f Pos) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_DEBUG_MeshMarkers.at(DT).TransSGN.translation(Pos);
	}//DEBUG_placeMeshMarker

	void ViewportManager::DEBUG_showMeshMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (DT == DatasetType::NONE) throw CForgeExcept("Invalid DatasetType");
		m_Viewports[VPIndex]->DEBUG_ShowDebugMeshMarker.at(DT) = Show;
	}//DEBUG_showMeshMarkerOnDataset

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// internal methods (private)
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int32_t ViewportManager::addViewport(int VPPositionX, int VPPositionY, int VPSizeX, int VPSizeY, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries) {
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
			pView->ShowDefaultVertexHoverMarker.insert(std::pair<DatasetType, bool>(Dataset.first, false));
			pView->ShowDefaultVertexSelectMarker.insert(std::pair<DatasetType, bool>(Dataset.first, false));
			pView->DEBUG_ShowDebugMeshMarker.insert(std::pair<DatasetType, bool>(Dataset.first, false)); //TODO: comment/remove when done with testing
			pView->ShowFeatMarkers.insert(std::pair<DatasetType, bool>(Dataset.first, false));
			pView->ShowCorrMarkers.insert(std::pair<DatasetType, bool>(Dataset.first, false));
		}

		m_Viewports[VPIdx] = pView;
		return VPIdx;
	}//addViewport

	void ViewportManager::addDatasetDisplayData(Viewport* pVP, DatasetType DT, DatasetGeometryType GT) {
		if (pVP == nullptr) throw NullpointerExcept("pVP");
		if (DT == DatasetType::NONE) throw CForgeExcept("DatasetType not initialized");

		auto Inserted = pVP->DatasetDisplayData.insert(std::pair<DatasetType, DatasetDisplayData*>(DT, nullptr));
		if (!Inserted.second) throw CForgeExcept("Specified DatasetType already exists!");

		DatasetDisplayData* pNewDisplayData = new DatasetDisplayData();
		pNewDisplayData->pPrimitivesActor = new DatasetActor();
		DatasetRenderMode RM = (GT == DatasetGeometryType::MESH) ? DatasetRenderMode::FILL : DatasetRenderMode::POINT;

		std::vector<Vector3f> Colors;
		switch (DT) {
		case DatasetType::TEMPLATE:
		case DatasetType::DTEMPLATE: for (uint32_t i = 0; i < m_DatasetModels.at(DT).vertexCount(); ++i) Colors.push_back(m_GlobalState.DefaultTemplateColor); break;
		case DatasetType::TARGET: for (uint32_t i = 0; i < m_DatasetModels.at(DT).vertexCount(); ++i) Colors.push_back(m_GlobalState.DefaultTargetColor); break;
		default: break;
		}
		m_DatasetModels.at(DT).colors(&Colors);
		Colors.clear();

		pNewDisplayData->pPrimitivesActor->init(&m_DatasetModels.at(DT), RM);

		if (GT == DatasetGeometryType::MESH) {
			pNewDisplayData->pWireframeActor = new DatasetActor();
		
			for (uint32_t i = 0; i < m_DatasetModels.at(DT).vertexCount(); ++i) 
				Colors.push_back(Vector3f(0.0f, 0.0f, 0.0f));
			
			m_DatasetModels.at(DT).colors(&Colors);
			Colors.clear();

			pNewDisplayData->pWireframeActor->init(&m_DatasetModels.at(DT), DatasetRenderMode::LINE);
		}
		else {
			pNewDisplayData->pWireframeActor = nullptr;
		}
		
		// set rotation of newly added dataset to match existing ones
		Quaternionf Rotation = Quaternionf::Identity();
		if (pVP->DatasetDisplayData.size() > 1) Rotation = pVP->DatasetDisplayData.begin()->second->TransSGN.rotation();

		pNewDisplayData->TransSGN.init(&(pVP->RootSGN), Vector3f::Zero(), Rotation, Vector3f::Ones());
		pNewDisplayData->PrimitivesGeomSGN.init(&(pNewDisplayData->TransSGN), pNewDisplayData->pPrimitivesActor);
		if (GT == DatasetGeometryType::MESH) {
			pNewDisplayData->WireframeGeomSGN.init(&(pNewDisplayData->TransSGN), pNewDisplayData->pWireframeActor);
			pNewDisplayData->WireframeGeomSGN.enable(true, false);
		}

		pNewDisplayData->TransSGN.enable(true, false);
		
		pVP->DatasetDisplayData.at(DT) = pNewDisplayData;
	}//addDatasetDisplayData

	void ViewportManager::clearDatasetDisplayData(size_t VPIndex) {
		auto pView = m_Viewports[VPIndex];

		for (auto it = pView->DatasetDisplayData.begin(); it != pView->DatasetDisplayData.end(); ++it) {
			if (it->second == nullptr) continue;

			pView->RootSGN.removeChild(&(it->second->TransSGN));

			it->second->PrimitivesGeomSGN.clear();
			it->second->WireframeGeomSGN.clear();
			it->second->TransSGN.clear();
			it->second->pPrimitivesActor->release();
			if (it->second->pWireframeActor != nullptr)
				it->second->pWireframeActor->release();

			delete it->second;
			it->second = nullptr;
		}
		pView->DatasetDisplayData.clear();
	}//clearDatasetDisplayData

	//std::vector<Vector3f> ViewportManager::calculateHausdorffVertexColors(std::vector<float>& HausdorffVals) { //TODO
	//	//TODO
	//}//calculateHausdorffVertexColors

	Quaternionf ViewportManager::arcballRotation(size_t VPIndex, Vector2f CursorPosStartOGL, Vector2f CursorPosEndOGL) {
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
	
	Vector3f ViewportManager::mapToSphereHyperbolic(Vector2f CursorPosOGL, Vector2i VPOffset, Vector2i VPSize) {
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