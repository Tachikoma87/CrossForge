#include "ViewportManager.h"

#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../Examples/SceneUtilities.hpp"

namespace TempReg {
	
	ViewportManager::ViewportManager(size_t MaxViewportCount) :
		m_MaxViewportCount(MaxViewportCount), m_ActiveViewportArrangement(ViewportArrangementType::ONE_FULLSCREEN) {

		m_GlobalState.DisplayCorrespondences = false;
		m_GlobalState.GlobalMarkers = false;
		m_GlobalState.MarkerDisplayMode = MarkerMode::DISABLED;
		m_GlobalState.DefaultTemplateColor = Vector3f(1.0f, 0.666f, 0.498f);	// light orange
		m_GlobalState.DefaultTargetColor = Vector3f(0.541f, 0.784f, 1.0f);		// light blue

		m_CorrespondenceMarkerGroups.reserve(128);
	}//Constructor

	ViewportManager::~ViewportManager() {
		for (auto& it : m_DatasetModels) it.second.clear();		
		m_DatasetModels.clear();
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

	void ViewportManager::loadViewports(const std::vector<Vector4f>& Tiles) {
		
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
			Viewport* View = m_Viewports[i];
			
			if (CursorPosOGL.x() >= (float)View->VP.Position.x() && CursorPosOGL.x() <= (float)View->VP.Position.x() + (float)View->VP.Size.x()) {
				if (CursorPosOGL.y() >= (float)View->VP.Position.y() && CursorPosOGL.y() <= (float)View->VP.Position.y() + (float)View->VP.Size.y()) {
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
		auto pView = m_Viewports[VPIndex];

		//process local (per viewport) & global view state
		pView->SG.update(FPSScale);
	}//updateViewport

	void ViewportManager::renderViewport(size_t VPIndex, CForge::RenderDevice* pRDev) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->SG.render(pRDev);
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

		for (auto& Dataset : m_Viewports[VPIndex]->ActiveDisplayData)
			Dataset.second->DatasetTransSGN.rotation(Rotation * Dataset.second->DatasetTransSGN.rotation());
	}//viewportArcballRotate

	void ViewportManager::acrballRotateAllViewports(size_t VPIndex, Vector2f Start, Vector2f End) {//TODO
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		Quaternionf Rotation = arcballRotation(VPIndex, Start, End);

		for (auto View : m_Viewports) {
			if (View == nullptr) continue;
			for (auto& Dataset : View->ActiveDisplayData)
				Dataset.second->DatasetTransSGN.rotation(Rotation * Dataset.second->DatasetTransSGN.rotation());
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

	void ViewportManager::addDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (itDatasetGeom->second->vertexCount() == 0) throw CForgeExcept("Dataset has no vertex data. Can not visualize it!");
		
		auto View = m_Viewports[VPIndex];
		auto Inserted = View->ActiveDisplayData.insert(std::pair<DatasetType, DatasetDisplayData*>(itDatasetGeom->first, nullptr));
		if (!Inserted.second) throw CForgeExcept("Specified DatasetType already exists!");
		
		DatasetDisplayData* pNewDisplayData = new DatasetDisplayData();
		pNewDisplayData->pDatasetActor = new DatasetActor();
		DatasetRenderMode RM = (itDatasetGeom->second->geometryType() == DatasetGeometryType::MESH) ? DatasetRenderMode::FILL : DatasetRenderMode::POINT;
		pNewDisplayData->pDatasetActor->init(&m_DatasetModels.at(itDatasetGeom->first), RM);

		// set rotation of newly added dataset to match existing ones
		Quaternionf Rotation = Quaternionf::Identity();
		if (View->ActiveDisplayData.size() > 1) Rotation = View->ActiveDisplayData.begin()->second->DatasetTransSGN.rotation();
		
		pNewDisplayData->DatasetTransSGN.init(&(View->RootSGN), Vector3f::Zero(), Rotation, Vector3f::Ones());
		pNewDisplayData->DatasetGeomSGN.init(&(pNewDisplayData->DatasetTransSGN), pNewDisplayData->pDatasetActor);
		
		View->ActiveDisplayData.at(itDatasetGeom->first) = pNewDisplayData;
	}//addDatasetDisplayData

	void ViewportManager::removeDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto View = m_Viewports[VPIndex];

		if (View->ActiveDisplayData.count(itDatasetGeom->first) == 0) return; // nothing to do

		View->RootSGN.removeChild(&(View->ActiveDisplayData.at(itDatasetGeom->first)->DatasetTransSGN));

		View->ActiveDisplayData.at(itDatasetGeom->first)->DatasetGeomSGN.clear();
		View->ActiveDisplayData.at(itDatasetGeom->first)->DatasetTransSGN.clear();
		View->ActiveDisplayData.at(itDatasetGeom->first)->pDatasetActor->release();

		delete View->ActiveDisplayData.at(itDatasetGeom->first);
		View->ActiveDisplayData.at(itDatasetGeom->first) = nullptr;
		View->ActiveDisplayData.erase(itDatasetGeom->first);
	}//removeDatasetDisplayData

	void ViewportManager::clearDatasetDisplayData(size_t VPIndex) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto View = m_Viewports[VPIndex];

		for (auto it = View->ActiveDisplayData.begin(); it != View->ActiveDisplayData.end(); ++it) {
			if (it->second == nullptr) continue;

			View->RootSGN.removeChild(&(it->second->DatasetTransSGN));

			it->second->DatasetGeomSGN.clear();
			it->second->DatasetTransSGN.clear();
			it->second->pDatasetActor->release();
			
			delete it->second;
			it->second = nullptr;
		}
		View->ActiveDisplayData.clear();
	}//removeAllDatasetDisplayData

	void ViewportManager::updateDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom) { //TODO
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		
		auto View = m_Viewports[VPIndex];

		//TODO...

	}//updateDatasetDisplayData

	void ViewportManager::showDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, bool Show) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto View = m_Viewports[VPIndex];
		View->ActiveDisplayData.at(itDatasetGeom->first)->DatasetGeomSGN.enable(true, Show);
	}//showDatasetDisplayData

	void ViewportManager::setSolidShading(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		Viewport* View = m_Viewports[VPIndex];
		auto DType = itDatasetGeom->first;
		std::vector<Vector3f> Colors;

		switch (DType) {
		case DatasetType::TEMPLATE:
		case DatasetType::DTEMPLATE: for (uint32_t i = 0; i < itDatasetGeom->second->vertexCount(); ++i) Colors.push_back(m_GlobalState.DefaultTemplateColor); break;
		case DatasetType::TARGET: for (uint32_t i = 0; i < itDatasetGeom->second->vertexCount(); ++i) Colors.push_back(m_GlobalState.DefaultTargetColor); break;
		case DatasetType::NONE: throw CForgeExcept("DatasetType not initialized"); break;
		default: break;
		}

		m_DatasetModels.at(itDatasetGeom->first).colors(&Colors);
		Colors.clear();

		//update DatasetActor
		auto* DisplayData = View->ActiveDisplayData.at(itDatasetGeom->first);
		//DisplayData->DatasetTransSGN.removeChild(&(DisplayData->DatasetGeomSGN));
		//DisplayData->DatasetGeomSGN.clear();
		DisplayData->pDatasetActor->release();

		DisplayData->pDatasetActor = new DatasetActor();
		DatasetRenderMode RM = (itDatasetGeom->second->geometryType() == DatasetGeometryType::MESH) ? DatasetRenderMode::FILL : DatasetRenderMode::POINT;
		DisplayData->pDatasetActor->init(&m_DatasetModels.at(itDatasetGeom->first), RM);
		//DisplayData->DatasetGeomSGN.init(&(DisplayData->DatasetTransSGN), DisplayData->pDatasetActor);
		DisplayData->DatasetGeomSGN.actor(DisplayData->pDatasetActor);
	}//setSolidShading
	
	void ViewportManager::setHausdorffShading(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, std::vector<Vector3f>& VertexColors) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (VertexColors.size() != itDatasetGeom->second->vertexCount()) throw CForgeExcept("Size mismatch: VertexColors.size() != itDatasetGeom->second.vertexCount()");
		
		Viewport* View = m_Viewports[VPIndex];
		m_DatasetModels.at(itDatasetGeom->first).colors(&VertexColors);

		//update DatasetActor
		auto* DisplayData = View->ActiveDisplayData.at(itDatasetGeom->first);
		//DisplayData->DatasetTransSGN.removeChild(&(DisplayData->DatasetGeomSGN));
		//DisplayData->DatasetGeomSGN.clear();
		DisplayData->pDatasetActor->release();
		
		DisplayData->pDatasetActor = new DatasetActor();
		DatasetRenderMode RM = (itDatasetGeom->second->geometryType() == DatasetGeometryType::MESH) ? DatasetRenderMode::FILL : DatasetRenderMode::POINT;
		DisplayData->pDatasetActor->init(&m_DatasetModels.at(itDatasetGeom->first), RM);
		//DisplayData->DatasetGeomSGN.init(&(DisplayData->DatasetTransSGN), DisplayData->pDatasetActor);
		DisplayData->DatasetGeomSGN.actor(DisplayData->pDatasetActor);
	}//setHausdorffShading

	void ViewportManager::renderMode(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, DatasetRenderMode RM) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if ((RM == DatasetRenderMode::FILL || RM == DatasetRenderMode::LINE) && itDatasetGeom->second->geometryType() == DatasetGeometryType::POINTCLOUD) 
			throw CForgeExcept("Incompatible DatasetRenderMode used: FILL/LINE on point cloud");

		Viewport* View = m_Viewports[VPIndex];
		View->ActiveDisplayData.at(itDatasetGeom->first)->pDatasetActor->renderMode(RM);
	}//renderMode

	void ViewportManager::setDatasetDisplayDataArrangement(size_t VPIndex, DisplayDataArrangementMode Arrangement) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto View = m_Viewports[VPIndex];
		View->DisplayDataArrangement = Arrangement;
		if (Arrangement == DisplayDataArrangementMode::LAYERED) arrangeDatasetDisplayDataLayered(VPIndex);
		else arrangeDatasetDisplayDataSideBySide(VPIndex);
	}//setDatasetDisplayDataArrangement

	void ViewportManager::arrangeDatasetDisplayDataLayered(size_t VPIndex) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto View = m_Viewports[VPIndex];

		for (auto it = View->ActiveDisplayData.begin(); it != View->ActiveDisplayData.end(); ++it)
			it->second->DatasetTransSGN.translation(Vector3f::Zero());
		
		View->DisplayDataArrangement = DisplayDataArrangementMode::LAYERED;
	}//arrangeDatasetDisplayDataLayered

	void ViewportManager::arrangeDatasetDisplayDataSideBySide(size_t VPIndex) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto View = m_Viewports[VPIndex];

		// update positions of all existing datasets
		// -> sum up x-axes of all bounding boxes
		float XWidthTotal = 0.0f;
		for (auto it = View->ActiveDisplayData.begin(); it != View->ActiveDisplayData.end(); ++it)
			XWidthTotal += m_DatasetModels.at(it->first).aabb().Max.x() - m_DatasetModels.at(it->first).aabb().Min.x();

		// -> center arrangement on world space origin
		float XStart = -(XWidthTotal * 0.5f);
		auto it = View->ActiveDisplayData.find(DatasetType::TEMPLATE);
		if (it != View->ActiveDisplayData.end()) {
			auto BB = m_DatasetModels.at(it->first).aabb();
			Vector3f Position = Vector3f(XStart + ((BB.Max.x() - BB.Min.x()) * 0.5f), 0.0f, 0.0f);
			it->second->DatasetTransSGN.translation(Position);

			XStart += (BB.Max.x() - BB.Min.x());
		}

		it = View->ActiveDisplayData.find(DatasetType::DTEMPLATE);
		if (it != View->ActiveDisplayData.end()) {
			auto BB = m_DatasetModels.at(it->first).aabb();
			Vector3f Position = Vector3f(XStart + ((BB.Max.x() - BB.Min.x()) * 0.5f), 0.0f, 0.0f);
			it->second->DatasetTransSGN.translation(Position);

			XStart += (BB.Max.x() - BB.Min.x());
		}

		it = View->ActiveDisplayData.find(DatasetType::TARGET);
		if (it != View->ActiveDisplayData.end()) {
			auto BB = m_DatasetModels.at(it->first).aabb();
			Vector3f Position = Vector3f(XStart + ((BB.Max.x() - BB.Min.x()) * 0.5f), 0.0f, 0.0f);
			it->second->DatasetTransSGN.translation(Position);
		}

		View->DisplayDataArrangement = DisplayDataArrangementMode::SIDE_BY_SIDE;
	}//arrangeDatasetDispalyDataSideBySide

	DisplayDataArrangementMode ViewportManager::activeDatasetDisplayDataArrangement(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		return m_Viewports[VPIndex]->DisplayDataArrangement;
	}//activeDatasetDisplayDataArrangement

	std::vector<DatasetType> ViewportManager::activeDatasetTypes(size_t VPIndex) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		Viewport* View = m_Viewports[VPIndex];
		std::vector<DatasetType> ActiveTypes;

		for (auto it = View->ActiveDisplayData.begin(); it != View->ActiveDisplayData.end(); ++it)
			ActiveTypes.push_back(it->first);
		
		return ActiveTypes;
	}//activeDatasetTypes

	Matrix4f ViewportManager::datasetModelMatrix(size_t VPIndex, DatasetType DT) const {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		
		auto View = m_Viewports[VPIndex];
		if (View->ActiveDisplayData.count(DT) == 0) {
			std::string Type;
			if (DT == DatasetType::TEMPLATE) Type = "TEMPLATE";
			if (DT == DatasetType::DTEMPLATE) Type = "DTEMPLATE";
			if (DT == DatasetType::TARGET) Type = "TARGET";
			throw CForgeExcept("Dataset " + Type + " not active");
		}

		Matrix4f TransMat = Matrix4f::Identity();
		TransMat.block<3, 1>(0, 3) = View->ActiveDisplayData.at(DT)->DatasetTransSGN.translation();

		Matrix4f RotMat = Matrix4f::Identity();
		Matrix3f RotQuatToMat = View->ActiveDisplayData.at(DT)->DatasetTransSGN.rotation().toRotationMatrix();
		RotMat.block<3, 3>(0, 0) = RotQuatToMat/*.transpose()*/;

		Matrix4f ScaleMat = Matrix4f::Identity();
		Vector3f Scale = View->ActiveDisplayData.at(DT)->DatasetTransSGN.scale();
		ScaleMat(0, 0) = Scale(0);
		ScaleMat(1, 1) = Scale(1);
		ScaleMat(2, 2) = Scale(2);

		Matrix4f Model = TransMat * RotMat * ScaleMat;
		return Model;
	}//datasetModelMatrix

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Vertex marker interactions
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ViewportManager::initVertexMarkerModels(std::string Filepath) {
		CForge::T3DMesh<float> RawMesh;
		CForge::SAssetIO::load(Filepath, &RawMesh);
		CForge::SceneUtilities::setMeshShader(&RawMesh, 0.4f, 0.0f);
		RawMesh.computePerVertexNormals();

		Vector3f MarkerColor;
		std::vector<Vector3f> Colors;

		// Initialize marker model for MarkerMode::DEFAULT_HOVER
		MarkerColor = Vector3f(1.0f, 1.0f, 1.0f); // white #ffffff
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors.push_back(MarkerColor);
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerMode, CForge::StaticActor*>(MarkerMode::DEFAULT_HOVER, nullptr));
		CForge::StaticActor* pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerMode::DEFAULT_HOVER) = pNewMarkerActor;

		// Initialize marker model for MarkerMode::DEFAULT_SELECTION
		MarkerColor = Vector3f(1.0f, 0.956f, 0.478f); // light yellow #fff47a
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = MarkerColor;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerMode, CForge::StaticActor*>(MarkerMode::DEFAULT_SELECTION, nullptr));
		pNewMarkerActor = new CForge::StaticActor(); 
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerMode::DEFAULT_SELECTION) = pNewMarkerActor;

		// Initialize marker model for MarkerMode::CORRESPONDENCE_PAIRS_IDLE
		MarkerColor = Vector3f(0.980f, 0.772f, 0.0f); // orange #fac500
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = MarkerColor;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerMode, CForge::StaticActor*>(MarkerMode::CORRESPONDENCE_PAIRS_IDLE, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerMode::CORRESPONDENCE_PAIRS_IDLE) = pNewMarkerActor;

		// Initialize marker model for MarkerMode::CORRESPONDENCE_PAIRS_SELECTION
		MarkerColor = Vector3f(1.0f, 0.956f, 0.478f); // light yellow #fff47a
		for (uint32_t i = 0; i < RawMesh.vertexCount(); i++) Colors[i] = MarkerColor;
		RawMesh.colors(&Colors);
		m_MarkerActors.insert(std::pair<MarkerMode, CForge::StaticActor*>(MarkerMode::CORRESPONDENCE_PAIRS_SELECTION, nullptr));
		pNewMarkerActor = new CForge::StaticActor();
		pNewMarkerActor->init(&RawMesh);
		m_MarkerActors.at(MarkerMode::CORRESPONDENCE_PAIRS_SELECTION) = pNewMarkerActor;
	}//initVertexMarkers

	void ViewportManager::placeSimpleMarker(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, MarkerMode MM, uint32_t VertexID) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		if (MM != MarkerMode::DEFAULT_HOVER && MM != MarkerMode::DEFAULT_SELECTION) throw CForgeExcept("MarkerMode incompatible");

		auto View = m_Viewports[VPIndex];

		if (View->DefaultMarkers.at(MM).SGData.TransSGN.parent() == nullptr) { // marker not yet initialized
			View->DefaultMarkers.at(MM).SGData.TransSGN.init(&(View->ActiveDisplayData.at(itDatasetGeom->first)->DatasetTransSGN), itDatasetGeom->second->vertex(VertexID), 
				Quaternionf::Identity(), Vector3f(0.1f, 0.1f, 0.1f));
			View->DefaultMarkers.at(MM).SGData.GeomSGN.init(&(View->DefaultMarkers.at(MM).SGData.TransSGN), m_MarkerActors.at(MM));
			View->DefaultMarkers.at(MM).DT = itDatasetGeom->first;
		}
		else {// marker is already initialized
			if (View->DefaultMarkers.at(MM).DT == itDatasetGeom->first) { // move to new vertex on same dataset
				View->DefaultMarkers.at(MM).SGData.TransSGN.translation(itDatasetGeom->second->vertex(VertexID));
			}
			else { // move to new vertex on different dataset
				View->ActiveDisplayData.at(View->DefaultMarkers.at(MM).DT)->DatasetTransSGN.removeChild(&(View->DefaultMarkers.at(MM).SGData.TransSGN));
				View->ActiveDisplayData.at(itDatasetGeom->first)->DatasetTransSGN.addChild(&(View->DefaultMarkers.at(MM).SGData.TransSGN));
				View->DefaultMarkers.at(MM).SGData.TransSGN.translation(itDatasetGeom->second->vertex(VertexID));
				View->DefaultMarkers.at(MM).DT = itDatasetGeom->first;
			}
		}		
	}//placeSimpleMarker
	
	void ViewportManager::hideSimpleMarker(size_t VPIndex, MarkerMode MM, bool Hide) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");
		m_Viewports[VPIndex]->DefaultMarkers.at(MM).SGData.GeomSGN.enable(true, Hide);
	}//hideSimpleMarker
	
	void ViewportManager::removeSimpleMarker(size_t VPIndex, MarkerMode MM) {
		if (VPIndex > activeViewportCount()) throw IndexOutOfBoundsExcept("VPIndex");

		auto View = m_Viewports[VPIndex];
		View->ActiveDisplayData.at(View->DefaultMarkers.at(MM).DT)->DatasetTransSGN.removeChild(&(View->DefaultMarkers.at(MM).SGData.TransSGN));
		View->DefaultMarkers.at(MM).SGData.GeomSGN.clear();
		View->DefaultMarkers.at(MM).SGData.TransSGN.clear();
		View->DefaultMarkers.at(MM).DT = DatasetType::NONE;
	}//removeSimpleMarker
	
	int64_t ViewportManager::addCorrespondenceMarkerGroupTemplate(std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, uint32_t VertexID, int64_t CorrMarkerGroup = (int64_t)-1) {		
		if (CorrMarkerGroup < -1 || CorrMarkerGroup > m_CorrespondenceMarkerGroups.size()) throw IndexOutOfBoundsExcept("CorrMarkerGroup");
		
		int64_t CGID = -1;		
		if (CorrMarkerGroup == -1) { // create new correspondence marker group
			if (!m_FreeCorrMarkerGroups.empty()) {
				CGID = m_FreeCorrMarkerGroups.top();
				m_FreeCorrMarkerGroups.pop();
			}
			else {
				CGID = m_CorrespondenceMarkerGroups.size();
				m_CorrespondenceMarkerGroups.push_back(CorrespondenceMarkerGroup());
			}
			m_CorrespondenceMarkerGroups[CGID].MM = MarkerMode::CORRESPONDENCE_PAIRS_SELECTION;
			m_CorrespondenceMarkerGroups[CGID].TemplateVertexID = VertexID;
			m_CorrespondenceMarkerGroups[CGID].ReadyForUse = false;
		}
		else { // add to existing correspondence marker group
			m_CorrespondenceMarkerGroups[CorrMarkerGroup].MM = MarkerMode::CORRESPONDENCE_PAIRS_SELECTION;
			m_CorrespondenceMarkerGroups[CorrMarkerGroup].TemplateVertexID = VertexID;
			m_CorrespondenceMarkerGroups[CorrMarkerGroup].ReadyForUse = true;

			for (auto View : m_Viewports)
				View->NewCorrMarkerGroups.push(CorrMarkerGroup);
		}
		return CGID;
	}//addCorrespondenceMarkerGroupTemplate
	
	int64_t ViewportManager::addCorrespondenceMarkerGroupTarget(std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, uint32_t VertexID, int64_t CorrMarkerGroup = (int64_t)-1) {
		if (CorrMarkerGroup < -1 || CorrMarkerGroup > m_CorrespondenceMarkerGroups.size()) throw IndexOutOfBoundsExcept("CorrMarkerGroup");
		
		int64_t CGID = -1;
		if (CorrMarkerGroup == -1) { // create new correspondence marker group
			if (!m_FreeCorrMarkerGroups.empty()) {
				CGID = m_FreeCorrMarkerGroups.top();
				m_FreeCorrMarkerGroups.pop();
			}
			else {
				CGID = m_CorrespondenceMarkerGroups.size();
				m_CorrespondenceMarkerGroups.push_back(CorrespondenceMarkerGroup());
			}			
			m_CorrespondenceMarkerGroups[CGID].MM = MarkerMode::CORRESPONDENCE_PAIRS_SELECTION;
			m_CorrespondenceMarkerGroups[CGID].TargetVertexID = VertexID;
			m_CorrespondenceMarkerGroups[CGID].ReadyForUse = false;
		}
		else { // add to existing correspondence marker group
			m_CorrespondenceMarkerGroups[CorrMarkerGroup].MM = MarkerMode::CORRESPONDENCE_PAIRS_SELECTION;
			m_CorrespondenceMarkerGroups[CorrMarkerGroup].TargetVertexID = VertexID;
			m_CorrespondenceMarkerGroups[CorrMarkerGroup].ReadyForUse = true;

			for (auto View : m_Viewports)
				View->NewCorrMarkerGroups.push(CorrMarkerGroup);
		}
		return CGID;
	}//addCorrespondenceMarkerGroupTarget
	
	void ViewportManager::removeCorrespondenceMarkerGroup(int64_t CorrMarkerGroup) {
		if (CorrMarkerGroup < 0 || CorrMarkerGroup > m_CorrespondenceMarkerGroups.size()) throw IndexOutOfBoundsExcept("CorrMarkerGroup");

		m_CorrespondenceMarkerGroups[CorrMarkerGroup].MM = MarkerMode::DISABLED;
		m_CorrespondenceMarkerGroups[CorrMarkerGroup].TemplateVertexID = 0;
		m_CorrespondenceMarkerGroups[CorrMarkerGroup].TargetVertexID = 0;
		m_CorrespondenceMarkerGroups[CorrMarkerGroup].TargetMarkerPos = Vector3f::Zero();
		m_CorrespondenceMarkerGroups[CorrMarkerGroup].ReadyForUse = false;
		m_FreeCorrMarkerGroups.push(CorrMarkerGroup);
	}
	
	void ViewportManager::removeCorrespondenceMarkerSGData(size_t VPIndex, int64_t CorrMarkerGroup) { //TODO

	}

	void ViewportManager::loadCorrespondenceMarkers(size_t VPIndex) { //TODO

	}

	void ViewportManager::displayCorrespondenceMarkers(size_t VPIndex, bool Show) { //TODO

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// internal methods (private)
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int32_t ViewportManager::addViewport(int VPPositionX, int VPPositionY, int VPSizeX, int VPSizeY) {		
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
		pView->DisplayDataArrangement = DisplayDataArrangementMode::SIDE_BY_SIDE;

		pView->DefaultMarkers.insert(std::pair<MarkerMode, SimpleMarker>(MarkerMode::DEFAULT_HOVER, SimpleMarker()));
		pView->DefaultMarkers.insert(std::pair<MarkerMode, SimpleMarker>(MarkerMode::DEFAULT_SELECTION, SimpleMarker()));

		pView->Cam.init(Vector3f(0.0f, 0.0f, 15.0f), Vector3f::UnitY());
		pView->Cam.projectionMatrix(pView->VP.Size.x(), pView->VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		m_Viewports[Idx] = pView;

		return Idx;
	}//addViewport

	void ViewportManager::removeViewport(size_t VPIndex) {
		if (VPIndex > m_Viewports.size()) throw IndexOutOfBoundsExcept("VPIndex");
		if (m_Viewports[VPIndex] == nullptr) return;

		clearDatasetDisplayData(VPIndex);

		m_Viewports[VPIndex]->RootSGN.clear();
		m_Viewports[VPIndex]->SG.clear();

		delete m_Viewports[VPIndex];
		m_Viewports[VPIndex] = nullptr;
	}//removeViewport

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