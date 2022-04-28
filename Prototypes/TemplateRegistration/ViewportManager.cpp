#include "ViewportManager.h"

#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../Examples/SceneUtilities.hpp"

namespace TempReg {
	
	ViewportManager::ViewportManager(size_t MaxViewportCount) :
		m_MaxViewportCount(MaxViewportCount), m_ActiveViewportArrangement(ViewportArrangementType::ONE_FULLSCREEN) {

	}//Constructor

	ViewportManager::~ViewportManager() {

	}//Destructor

	//
	// Viewport interactions
	//

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

	void ViewportManager::loadViewports(const std::vector<Vector4f>& Tiles) {
		
		Vector4i Tile = Vector4i::Zero();
		size_t i = 0;

		// reuse existing viewports
		size_t ActiveVPCount = activeViewportCount();
		while (i < std::min(ActiveVPCount, Tiles.size())) {
			Tile = Tiles[i].array().ceil().cast<int>();
			m_Viewports[i]->VP.Position(0) = Tile(0);
			m_Viewports[i]->VP.Position(1) = Tile(1);
			m_Viewports[i]->VP.Size(0) = Tile(2);
			m_Viewports[i]->VP.Size(1) = Tile(3);		
			++i;
		}

		// add missing viewports
		while (i < Tiles.size()) {
			Tile = Tiles[i].array().ceil().cast<int>();
			addViewport(Tile(0), Tile(1), Tile(2), Tile(3));			
			++i;
		}

		//remove spare viewports
		
		while (i < ActiveVPCount) {
			removeViewport(i);
			++i;
		}
	}//loadViewports

	void ViewportManager::resizeActiveViewports(const std::vector<Vector4f>& Tiles) {
		if (Tiles.size() != m_Viewports.size()) throw IndexOutOfBoundsExcept("Tiles.size() != m_Viewports.size()");

		Vector4i Tile = Vector4i::Zero();
		
		for (size_t i = 0; i < Tiles.size(); ++i) {
			Tile = Tiles[i].array().ceil().cast<int>();
			m_Viewports[i]->VP.Position(0) = Tile(0);
			m_Viewports[i]->VP.Position(1) = Tile(1);
			m_Viewports[i]->VP.Size(0) = Tile(2);
			m_Viewports[i]->VP.Size(1) = Tile(3);
		}
	}//resizeActiveViewports

	void ViewportManager::updateViewport(size_t VPIndex, CForge::VirtualCamera* pCam, float FPSScale) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		auto pView = m_Viewports[VPIndex];
		
		//process local (per viewport) & global view state
		pCam->projectionMatrix(pView->VP.Size.x(), pView->VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);
		pView->SG.update(FPSScale);
	}//updateViewport

	void ViewportManager::renderViewport(size_t VPIndex, CForge::RenderDevice* pRDev) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->SG.render(pRDev);
	}//renderViewport

	Vector2i ViewportManager::viewportPosition(size_t VPIndex) const {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");
		return m_Viewports[VPIndex]->VP.Position;
	}//viewportPosition

	Vector2i ViewportManager::viewportSize(size_t VPIndex) const {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");
		return m_Viewports[VPIndex]->VP.Size;
	}//viewportSize

	Vector2f ViewportManager::viewportCenter(size_t VPIndex) const {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");

		Vector2f Pos((float)m_Viewports[VPIndex]->VP.Position.x(), (float)m_Viewports[VPIndex]->VP.Position.y());
		Vector2f Size((float)m_Viewports[VPIndex]->VP.Size.x(), (float)m_Viewports[VPIndex]->VP.Size.y());
		return (Pos + (Size / 2.0f));
	}//viewportCenter

	size_t ViewportManager::activeViewportCount(void) const {
		size_t Count = 0;
		for (size_t i = 0; i < m_Viewports.size(); ++i) {
			if (m_Viewports[i] != nullptr) ++Count;
		}
		return Count;
	}//activeViewportCount

	size_t ViewportManager::maxViewportCount(void) const {
		return m_MaxViewportCount;
	}//maxViewportCount

	CForge::RenderDevice::Viewport ViewportManager::getRenderDeviceViewport(size_t VPIndex) const {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) throw NullpointerExcept("m_Viewports[VPIndex]");
		return m_Viewports[VPIndex]->VP;
	}//getRenderDeviceViewport

	size_t ViewportManager::mouseInViewport(CForge::Mouse* pMouse) const {
		for (size_t i = 0; i < activeViewportCount(); ++i) {
			
			Viewport* View = m_Viewports[i];

			if (pMouse->position().x() >= View->VP.Position.x() && pMouse->position().x() <= View->VP.Position.x() + View->VP.Size.x()) {
				if (pMouse->position().y() >= View->VP.Position.y() && pMouse->position().y() <= View->VP.Position.y() + View->VP.Size.y()) {
					return i;
				}
			}
		}
		return -1;
	}//mouseInViewport

	//
	// Viewport viewing interactions
	//

	void ViewportManager::setCamProjectionMode(size_t VPIndex, DatasetViewProjectionMode Mode) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->ProjMode = Mode;
	}//setCamProjectionMode

	Quaternionf ViewportManager::arcballRotate(size_t VPIndex, bool FirstClick, uint32_t CursorX, uint32_t CursorY, const CForge::VirtualCamera* pCam) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");

		Viewport* View = m_Viewports[VPIndex];

		Quaternionf Rotation = View->ArcballControls.rotate(FirstClick, CursorX, CursorY, View->VP, pCam);
		View->RootSGN.rotation(Rotation * View->RootSGN.rotation());
		return Rotation;
	}//arcballRotate

	void ViewportManager::applyArcballRotation(size_t VPIndex, Quaternionf Rotation) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->RootSGN.rotation(Rotation * m_Viewports[VPIndex]->RootSGN.rotation());
	}//applyArcballRotation

	void ViewportManager::setCamPos(size_t VPIndex, Vector3f Eye) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitEye = Eye;
	}//setCamPos

	void ViewportManager::setCamTarget(size_t VPIndex, Vector3f Target) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitTarget = Target;
	}//setCamTarget

	void ViewportManager::setCamZoom(size_t VPIndex, float Zoom) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitZoom = Zoom;
	}//setCamZoom

	void ViewportManager::moveCamPos(size_t VPIndex, Vector3f Movement) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitEye += Movement;
	}//moveCamPos

	void ViewportManager::moveCamTarget(size_t VPIndex, Vector3f Movement) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitTarget += Movement;
	}//moveCamTarget

	void ViewportManager::zoomCam(size_t VPIndex, float Change) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->OrbitZoom += Change;
	}//zoomCam

	DatasetViewProjectionMode ViewportManager::getCamProjectionMode(size_t VPIndex) const {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->ProjMode;
	}//getCamProjectionMode

	Vector3f ViewportManager::getCamPos(size_t VPIndex) const {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->OrbitEye;
	}//getCamPos

	Vector3f ViewportManager::getCamTarget(size_t VPIndex) const {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->OrbitTarget;
	}//getCamTarget

	float ViewportManager::getCamZoom(size_t VPIndex) const {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->OrbitZoom;
	}//getCamZoom

	//
	// MeshDataset interactions
	//

	void ViewportManager::addDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset, Vector3f Position, Quaternionf Rotation, Vector3f Scale) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		if (itDataset->second.mesh()->vertexCount() == 0) throw CForgeExcept("Mesh has no vertex data. Can not visualize it!");
		
		Viewport* View = m_Viewports[VPIndex];

		if (View->ActiveDisplayData.count(itDataset->first) > 0) throw CForgeExcept("Specified dataset is already active in this dataset view!");

		View->ActiveDisplayData.insert(std::pair<DatasetType, DatasetDisplayData*>(itDataset->first, nullptr));

		DatasetDisplayData* pNewDisplayData = new DatasetDisplayData();
		pNewDisplayData->pDatasetActor = new CForge::StaticActor();
		pNewDisplayData->pDatasetActor->init(itDataset->second.mesh());
		pNewDisplayData->DatasetTransSGN.init(&(View->RootSGN), Position, Rotation, Scale);
		pNewDisplayData->DatasetGeomSGN.init(&(pNewDisplayData->DatasetTransSGN), pNewDisplayData->pDatasetActor);

		View->ActiveDisplayData.at(itDataset->first) = pNewDisplayData;

		//NewDatasetModel.VertexMarkerTransSGN.init(&(NewDatasetModel.DatasetGeomSGN), Position, Rotation);
		// TODO interactive vertex marker...?
		// TODO vertex markers...?
	}//addDataset

	void ViewportManager::removeDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");

		Viewport* View = m_Viewports[VPIndex];

		if (View->ActiveDisplayData.count(itDataset->first) == 0) return; // nothing to do

		View->RootSGN.removeChild(&(View->ActiveDisplayData.at(itDataset->first)->DatasetTransSGN));

		View->ActiveDisplayData.at(itDataset->first)->DatasetGeomSGN.clear();
		View->ActiveDisplayData.at(itDataset->first)->DatasetTransSGN.clear();
		View->ActiveDisplayData.at(itDataset->first)->pDatasetActor->release();
		// TODO interactive vertex marker...?
		// TODO vertex markers...?

		delete View->ActiveDisplayData.at(itDataset->first);
		View->ActiveDisplayData.at(itDataset->first) = nullptr;
		View->ActiveDisplayData.erase(itDataset->first);
	}//removeDataset

	void ViewportManager::removeAllDatasets(size_t VPIndex) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");

		Viewport* View = m_Viewports[VPIndex];

		for (auto itDataset = View->ActiveDisplayData.begin(); itDataset != View->ActiveDisplayData.end(); ++itDataset) {
			if (itDataset->second == nullptr) continue;

			View->RootSGN.removeChild(&(itDataset->second->DatasetTransSGN));

			itDataset->second->DatasetGeomSGN.clear();
			itDataset->second->DatasetTransSGN.clear();
			itDataset->second->pDatasetActor->release();
			// TODO interactive vertex marker...?
			// TODO vertex markers...?

			delete itDataset->second;
			itDataset->second = nullptr;
		}
		View->ActiveDisplayData.clear();
	}//removeAllDatasets

	void ViewportManager::updateDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		//TODO
	}//updateDataset

	void ViewportManager::showDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset, bool Rval) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		//TODO
	}//showDataset

	bool ViewportManager::datasetActive(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset) const {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");

		Viewport* View = m_Viewports[VPIndex];

		if (View->ActiveDisplayData.count(itDataset->first) > 0) return true;
		return false;
	}//datasetActive

	// temporary test
	void ViewportManager::TESTCameraRotateDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset, Quaternionf CamRot) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");

		Viewport* View = m_Viewports[VPIndex];
		DatasetDisplayData* D = View->ActiveDisplayData.at(itDataset->first);
		D->DatasetTransSGN.rotation(CamRot/* * D->DatasetTransSGN.rotation()*/);
	}

	//
	// Vertex marker interactions
	//

	void ViewportManager::initVertexMarkers(std::string Filepath) {
		CForge::SAssetIO::load(Filepath, &m_MarkerModelData);
		CForge::SceneUtilities::setMeshShader(&m_MarkerModelData, 0.4f, 0.0f);
		m_MarkerModelData.computePerVertexNormals();
		m_MarkerModelData.computeAxisAlignedBoundingBox();

		Vector3f MarkerColor;
		std::vector<Vector3f> Colors;
		Colors.reserve(m_MarkerModelData.vertexCount());

		// Initialize StaticActor for vertex marker mode: HOVER
		MarkerColor = Vector3f(1.0f, 0.956f, 0.478f); // light yellow #fff47a
		for (uint32_t i = 0; i < m_MarkerModelData.vertexCount(); i++) Colors.push_back(MarkerColor);
		m_MarkerModelData.colors(&Colors);

		m_GlobalState.m_MarkerActors.insert(std::pair<MarkerMode, MarkerActor*>(MarkerMode::HOVER, nullptr));

		MarkerActor* pMarkerActor = new MarkerActor();
		pMarkerActor->MarkerColor = MarkerColor;
		pMarkerActor->pActor = new CForge::StaticActor();
		pMarkerActor->pActor->init(&m_MarkerModelData);

		m_GlobalState.m_MarkerActors[MarkerMode::HOVER] = pMarkerActor;
		pMarkerActor = nullptr;

		// Initialize StaticActor for vertex marker mode: ACTIVE_SELECTION
		MarkerColor = Vector3f(0.980f, 0.772f, 0.0f); // orange #fac500
		for (uint32_t i = 0; i < m_MarkerModelData.vertexCount(); i++) Colors[i] = MarkerColor;
		m_MarkerModelData.colors(&Colors);

		m_GlobalState.m_MarkerActors.insert(std::pair<MarkerMode, MarkerActor*>(MarkerMode::ACTIVE_SELECTION, nullptr));

		pMarkerActor = new MarkerActor();
		pMarkerActor->MarkerColor = MarkerColor;
		pMarkerActor->pActor = new CForge::StaticActor();
		pMarkerActor->pActor->init(&m_MarkerModelData);

		m_GlobalState.m_MarkerActors[MarkerMode::ACTIVE_SELECTION] = pMarkerActor;
		pMarkerActor = nullptr;

		// Initialize StaticActor for vertex marker mode: CORRESPONDENCE_PAIRS_IDLE
		MarkerColor = Vector3f(1.0f, 0.0f, 0.0f); // TODO Farbe wählen!
		for (uint32_t i = 0; i < m_MarkerModelData.vertexCount(); i++) Colors[i] = MarkerColor;
		m_MarkerModelData.colors(&Colors);

		m_GlobalState.m_MarkerActors.insert(std::pair<MarkerMode, MarkerActor*>(MarkerMode::CORRESPONDENCE_PAIRS_IDLE, nullptr));

		pMarkerActor = new MarkerActor();
		pMarkerActor->MarkerColor = MarkerColor;
		pMarkerActor->pActor = new CForge::StaticActor();
		pMarkerActor->pActor->init(&m_MarkerModelData);

		m_GlobalState.m_MarkerActors[MarkerMode::CORRESPONDENCE_PAIRS_IDLE] = pMarkerActor;
		pMarkerActor = nullptr;
	}//initVertexMarkers

	void ViewportManager::markerMode(MarkerMode Mode) {
		m_GlobalState.MarkerDisplayMode = Mode;
	}//markerMode

	void ViewportManager::displayCorrespondences(bool Rval) {
		m_GlobalState.DisplayCorrespondences = Rval;
	}//correspondenceDisplayMode

	//
	// internal methods (private)
	//

	int32_t ViewportManager::addViewport(int VPPositionX, int VPPositionY, int VPSizeX, int VPSizeY) {
		//if (m_Viewports.size() >= m_MaxNumberViews) return -1; // max. allowed number of simultaneously active viewports reached, return -1
		
		int32_t Idx = -1;
		for (size_t i = 0; i < m_Viewports.size(); ++i) {
			if (m_Viewports[i] == nullptr) {
				Idx = i;
				break;
			}
		}
		if (Idx < 0) {
			Idx = m_Viewports.size();
			m_Viewports.push_back(nullptr);
		 }
		
		Viewport* pView = new Viewport();

		pView->VP.Position(0) = VPPositionX;
		pView->VP.Position(1) = VPPositionY;
		pView->VP.Size(0) = VPSizeX;
		pView->VP.Size(1) = VPSizeY;

		//TODO init default orbit controls

		// init scene graph root
		pView->RootSGN.init(nullptr);
		pView->SG.init(&pView->RootSGN);

		m_Viewports[Idx] = pView;

		return Idx;
	}//addViewport

	void ViewportManager::removeViewport(size_t VPIndex) {
		if (VPIndex < 0 || VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) return;

		removeAllDatasets(VPIndex);

		m_Viewports[VPIndex]->RootSGN.clear();
		m_Viewports[VPIndex]->SG.clear();

		delete m_Viewports[VPIndex];
		m_Viewports[VPIndex] = nullptr;
	}//removeViewport
}