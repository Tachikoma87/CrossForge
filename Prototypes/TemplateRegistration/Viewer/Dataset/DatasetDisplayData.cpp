#include "DatasetDisplayData.h"

#include "../../../CForge/AssetIO/SAssetIO.h"
#include "../../../Examples/SceneUtilities.hpp"

namespace TempReg {

	DatasetDisplayData::DatasetDisplayData(void) : 
		m_DatasetRenderMode(DatasetActor::DatasetRenderMode::FILL), 
		m_ShowAsSurface(true), 
		m_ShowAsWireframe(false), 
		m_pSurfaceActor(nullptr), 
		m_pSecondaryWireframeActor(nullptr), 
		m_ActiveColor(DatasetColor::SOLID_COLOR) {
		
		m_SurfaceGeomSGN.clear();
		m_SecondaryWireframeGeomSGN.clear();
		m_SingleMarker.clear();
		m_FeatCorrMarkers.clear();
		m_AutoCorrMarkers.clear();
	}//Constructor

	DatasetDisplayData::~DatasetDisplayData() {

	}//Destructor

	void DatasetDisplayData::initAsMesh(std::string Filepath, Vector3f SolidColor, Vector3f MarkerScale, bool ShowFeatCorrMarkers, bool ShowAutoCorrMarkers) {
		m_IsMesh = true;

		// init raw model data
		CForge::SAssetIO::load(Filepath, &m_RawModelData);
		CForge::SceneUtilities::setMeshShader(&m_RawModelData, 0.4f, 0.0f);
		m_RawModelData.computePerVertexNormals();
				
		m_ActiveColor = DatasetColor::SOLID_COLOR;
		m_SolidColor = SolidColor;
		m_FittingErrorColors.resize(m_RawModelData.vertexCount());

		std::vector<Vector3f> Colors;
		Colors.assign(m_RawModelData.vertexCount(), SolidColor);
		m_RawModelData.colors(&Colors);
		
		// init surface actor
		m_pSurfaceActor = new DatasetActor();
		m_pSurfaceActor->init(&m_RawModelData, DatasetActor::DatasetRenderMode::FILL);
		
		Colors.assign(m_RawModelData.vertexCount(), Vector3f::Zero()); // color of secondary wireframe for now hardcoded to black
		m_RawModelData.colors(&Colors);
		Colors.clear();

		// init secondary wireframe actor
		m_pSecondaryWireframeActor = new DatasetActor();
		m_pSecondaryWireframeActor->init(&m_RawModelData, DatasetActor::DatasetRenderMode::LINE);

		// init scene graph data
		m_SurfaceGeomSGN.init(nullptr, m_pSurfaceActor);
		m_SecondaryWireframeGeomSGN.init(nullptr, m_pSecondaryWireframeActor);
		m_SecondaryWireframeGeomSGN.enable(true, false);

		m_SingleMarker.init(nullptr, nullptr, Vector3f::Zero(), MarkerScale);
		m_SingleMarker.show(false);
		m_FeatCorrMarkers.init(nullptr, MarkerScale);
		m_FeatCorrMarkers.show(ShowFeatCorrMarkers);
		m_AutoCorrMarkers.init(nullptr, MarkerScale);
		m_AutoCorrMarkers.show(ShowAutoCorrMarkers);
	}//initMesh

	void DatasetDisplayData::initAsPointCloud(std::string Filepath, Vector3f SolidColor, Vector3f MarkerScale, bool ShowFeatCorrMarkers, bool ShowAutoCorrMarkers) {
		m_IsMesh = false;

		// init raw model data
		CForge::SAssetIO::load(Filepath, &m_RawModelData);
		CForge::SceneUtilities::setMeshShader(&m_RawModelData, 0.4f, 0.0f);
		m_RawModelData.computePerVertexNormals();
		
		m_ActiveColor = DatasetColor::SOLID_COLOR;
		m_SolidColor = SolidColor;
		m_FittingErrorColors.resize(m_RawModelData.vertexCount());

		std::vector<Vector3f> Colors;
		Colors.assign(m_RawModelData.vertexCount(), SolidColor);
		m_RawModelData.colors(&Colors);
		Colors.clear();

		// init surface actor
		m_pSurfaceActor = new DatasetActor();
		m_pSurfaceActor->init(&m_RawModelData, DatasetActor::DatasetRenderMode::POINT);

		// no secondary wireframe actor needed
		m_pSecondaryWireframeActor = nullptr;

		// init scene graph data
		m_SurfaceGeomSGN.init(nullptr, m_pSurfaceActor);

		m_SingleMarker.init(nullptr, nullptr, Vector3f::Zero(), MarkerScale);
		m_SingleMarker.show(false);
		m_FeatCorrMarkers.init(nullptr, MarkerScale);
		m_FeatCorrMarkers.show(ShowFeatCorrMarkers);
		m_AutoCorrMarkers.init(nullptr, MarkerScale);
		m_AutoCorrMarkers.show(ShowAutoCorrMarkers);
	}//initPointCloud

	void DatasetDisplayData::updateModelData(const Matrix<float, Dynamic, 3, RowMajor>& VertexPositions) {
		// update raw model data
		for (uint32_t i = 0; i < VertexPositions.rows(); ++i) m_RawModelData.vertex(i) = VertexPositions.row(i);

		if (m_IsMesh) m_RawModelData.computePerVertexNormals();
			
		// update m_pSurfaceActor and m_pSecondaryWireframeActor as necessary
		if (m_ActiveColor == DatasetColor::SOLID_COLOR) {
			std::vector<Vector3f> Colors;
			Colors.assign(m_RawModelData.vertexCount(), m_SolidColor);
			m_RawModelData.colors(&Colors);
			Colors.clear();
		}

		if (m_ActiveColor == DatasetColor::FITTING_ERROR_COLOR) {
			m_RawModelData.colors(&m_FittingErrorColors);
		}
		
		m_SurfaceGeomSGN.actor(nullptr);
		auto SurfActRM = m_pSurfaceActor->renderMode();
		m_pSurfaceActor->release();
		m_pSurfaceActor = new DatasetActor();
		m_pSurfaceActor->init(&m_RawModelData, SurfActRM);
		m_SurfaceGeomSGN.actor(m_pSurfaceActor);

		if (m_IsMesh) {
			std::vector<Vector3f> Colors;
			Colors.assign(m_RawModelData.vertexCount(), Vector3f::Zero()); // color of secondary wireframe for now hardcoded to black
			m_RawModelData.colors(&Colors);
			Colors.clear();

			m_SecondaryWireframeGeomSGN.actor(nullptr);
			m_pSecondaryWireframeActor->release();
			m_pSecondaryWireframeActor = new DatasetActor();
			m_pSurfaceActor->init(&m_RawModelData, DatasetActor::DatasetRenderMode::LINE);
			m_SecondaryWireframeGeomSGN.actor(m_pSecondaryWireframeActor);
		}
	}//updateModelData

	void DatasetDisplayData::addToSceneGraph(CForge::ISceneGraphNode* pParent) {		
		pParent->addChild(&m_SurfaceGeomSGN);

		if (m_pSecondaryWireframeActor != nullptr)
			pParent->addChild(&m_SecondaryWireframeGeomSGN);

		m_FeatCorrMarkers.addToSceneGraph(pParent);
		m_AutoCorrMarkers.addToSceneGraph(pParent);
		m_SingleMarker.addToSceneGraph(pParent);
	}//addToSceneGraph

	void DatasetDisplayData::removeFromSceneGraph(void) {
		if (m_SurfaceGeomSGN.parent() != nullptr)
			m_SurfaceGeomSGN.parent()->removeChild(&m_SurfaceGeomSGN);

		if (m_pSecondaryWireframeActor != nullptr) {
			if (m_SecondaryWireframeGeomSGN.parent() != nullptr)
				m_SecondaryWireframeGeomSGN.parent()->removeChild(&m_SecondaryWireframeGeomSGN);
		}

		m_FeatCorrMarkers.removeFromSceneGraph();
		m_AutoCorrMarkers.removeFromSceneGraph();
		m_SingleMarker.removeFromSceneGraph();
	}//removeFromSceneGraph

	void DatasetDisplayData::clear(void) {
		removeFromSceneGraph();

		m_SurfaceGeomSGN.clear();
		m_SecondaryWireframeGeomSGN.clear();
		m_pSurfaceActor->release();
		if (m_pSecondaryWireframeActor != nullptr) m_pSecondaryWireframeActor->release();

		m_FeatCorrMarkers.clear();
		m_AutoCorrMarkers.clear();
		m_SingleMarker.clear();
	}//clear

	void DatasetDisplayData::showDatasetAsSurface(bool Show) {
		if (m_DatasetRenderMode == DatasetActor::DatasetRenderMode::POINT) return; // skip point clouds
		m_ShowAsSurface = Show;
		m_SurfaceGeomSGN.enable(true, m_ShowAsSurface || m_ShowAsWireframe);
		m_SecondaryWireframeGeomSGN.enable(true, m_ShowAsSurface && m_ShowAsWireframe);
		m_pSurfaceActor->renderMode((!m_ShowAsSurface && m_ShowAsWireframe) ? DatasetActor::DatasetRenderMode::LINE : DatasetActor::DatasetRenderMode::FILL);
	}//showDatasetAsSurface
	
	const bool DatasetDisplayData::showDatasetAsSurface(void) const {
		return m_ShowAsSurface;
	}//showDatasetAsSurface
	
	void DatasetDisplayData::showDatasetAsWireframe(bool Show) {
		if (m_DatasetRenderMode == DatasetActor::DatasetRenderMode::POINT) return; // skip point clouds
		m_ShowAsWireframe = Show;
		m_SurfaceGeomSGN.enable(true, m_ShowAsSurface || m_ShowAsWireframe);
		m_SecondaryWireframeGeomSGN.enable(true, m_ShowAsSurface && m_ShowAsWireframe);
		m_pSurfaceActor->renderMode((!m_ShowAsSurface && m_ShowAsWireframe) ? DatasetActor::DatasetRenderMode::LINE : DatasetActor::DatasetRenderMode::FILL);
	}//showDatasetAsWireframe

	const bool DatasetDisplayData::showDatasetAsWireframe(void) const {
		return m_ShowAsWireframe;
	}//showDatasetAsWireframe

	void DatasetDisplayData::solidColor(Vector3f Color) {
		m_SolidColor = Color;
	}//solidColor

	void DatasetDisplayData::activateSolidColor(void) {
		if (m_ActiveColor == DatasetColor::SOLID_COLOR) return; // do nothing
		m_ActiveColor = DatasetColor::SOLID_COLOR;

		std::vector<Vector3f> Colors;
		Colors.assign(m_RawModelData.vertexCount(), m_SolidColor);
		m_RawModelData.colors(&Colors);
		Colors.clear();

		m_SurfaceGeomSGN.actor(nullptr);
		auto SurfActRM = m_pSurfaceActor->renderMode();
		m_pSurfaceActor->release();
		m_pSurfaceActor = new DatasetActor();
		m_pSurfaceActor->init(&m_RawModelData, SurfActRM);
		m_SurfaceGeomSGN.actor(m_pSurfaceActor);
	}//activateSolidColor

	void DatasetDisplayData::fittingErrorColors(std::vector<Vector3f>& VertexColors) {
		if (VertexColors.size() != m_FittingErrorColors.size()) throw CForgeExcept("VertexColors.size() != m_FittingErrorColor.size()");
		for (size_t i = 0; i < VertexColors.size(); ++i)
			m_FittingErrorColors[i] = VertexColors[i];
	}//fittingErrorColors
	
	void DatasetDisplayData::activateFittingErrorColors(void) {
		if (m_ActiveColor == DatasetColor::FITTING_ERROR_COLOR) return; // do nothing
		m_ActiveColor = DatasetColor::FITTING_ERROR_COLOR;
				
		m_RawModelData.colors(&m_FittingErrorColors);
		
		m_SurfaceGeomSGN.actor(nullptr);
		auto SurfActRM = m_pSurfaceActor->renderMode();
		m_pSurfaceActor->release();
		m_pSurfaceActor = new DatasetActor();
		m_pSurfaceActor->init(&m_RawModelData, SurfActRM);
		m_SurfaceGeomSGN.actor(m_pSurfaceActor);
	}//activateFittingErrorColors

	const DatasetDisplayData::DatasetColor DatasetDisplayData::activeDatasetColor(void) const {
		return m_ActiveColor;
	}//activeDatasetColor

	void DatasetDisplayData::placeSingleMarker(DatasetMarkerActor* pMarkerActor, Vector3f Pos) {
		m_SingleMarker.actor(pMarkerActor);
		m_SingleMarker.translation(Pos);
		m_SingleMarker.show(true);
	}//placeSingleMarker

	void DatasetDisplayData::hideSingleMarker(void) {
		m_SingleMarker.show(false);
	}//hideSingleMarker

	void DatasetDisplayData::addFeatureCorrMarker(size_t MarkerID, Vector3f Pos, DatasetMarkerActor* pMarkerActor) {
		m_FeatCorrMarkers.addMarkerInstance(MarkerID, Pos, pMarkerActor);
	}//addFeatureCorrMarker
	
	void DatasetDisplayData::removeFeatureCorrMarker(size_t MarkerID) {
		m_FeatCorrMarkers.removeMarkerInstance(MarkerID);
	}//removeFeatureCorrMarker
	
	void DatasetDisplayData::setFeatureCorrMarkerColor(size_t MarkerID, DatasetMarkerActor* pMarkerActor) {
		m_FeatCorrMarkers.markerActor(MarkerID, pMarkerActor);
	}//setFeatureCorrMarkerActor
	
	void DatasetDisplayData::setFeatureCorrMarkerPosition(size_t MarkerID, Vector3f Pos) {
		m_FeatCorrMarkers.markerPosition(MarkerID, Pos);
	}//setFeatureCorrMarkerPosition
	
	void DatasetDisplayData::showFeatureCorrMarkers(bool Show) {
		m_FeatCorrMarkers.show(Show);
	}//showFeatureCorrMarkers

	void DatasetDisplayData::addAutomaticCorrMarker(size_t MarkerID, Vector3f Pos, DatasetMarkerActor* pMarkerActor) {
		m_AutoCorrMarkers.addMarkerInstance(MarkerID, Pos, pMarkerActor);
	}//addAutomaticCorrMarker
	
	void DatasetDisplayData::removeAutomaticCorrMarker(size_t MarkerID) {
		m_AutoCorrMarkers.removeMarkerInstance(MarkerID);
	}//removeAutomaticCorrMarker
	
	void DatasetDisplayData::setAutomaticCorrMarkerColor(size_t MarkerID, DatasetMarkerActor* pMarkerActor) {
		m_AutoCorrMarkers.markerActor(MarkerID, pMarkerActor);
	}//setAutomaticCorrMarkerActor

	void DatasetDisplayData::setAutomaticCorrMarkerPosition(size_t MarkerID, Vector3f Pos) {
		m_AutoCorrMarkers.markerPosition(MarkerID, Pos);
	}//setAutomaticCorrMarkerPosition
	
	void DatasetDisplayData::showAutomaticCorrMarkers(bool Show) {
		m_AutoCorrMarkers.show(Show);
	}//showAutomaticCorrMarkers

	void DatasetDisplayData::clearFeatureCorrMarkers(void) {
		m_FeatCorrMarkers.clear();
	}//clearFeatureCorrMarkers

	void DatasetDisplayData::clearAutomaticCorrMarkers(void) {
		m_AutoCorrMarkers.clear();
	}//clearAutomaticCorrMarkers
}