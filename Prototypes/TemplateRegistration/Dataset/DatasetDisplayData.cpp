#include "DatasetDisplayData.h"

namespace TempReg {

	DatasetDisplayData::DatasetDisplayData(void) : 
		m_ShadingMode(DatasetShadingMode::SOLID_COLOR), m_pPrimitivesActor(nullptr),  m_pWireframeActor(nullptr) {
		m_TransSGN.clear();
		m_PrimitivesGeomSGN.clear();
		m_WireframeGeomSGN.clear();
	}//Constructor

	DatasetDisplayData::~DatasetDisplayData() {

	}//Destructor

	void DatasetDisplayData::init(
		DatasetType DT, DatasetGeometryType GT, CForge::T3DMesh<float>* pModel, Vector3f Color,
		CForge::ISceneGraphNode* pParent, Eigen::Vector3f Translation, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale) {
		initPrimitivesActor(GT, pModel, Color);
		
		if (GT == DatasetGeometryType::MESH) initWireframeActor(pModel, Vector3f(0.0f, 0.0f, 0.0f));
		else m_pWireframeActor = nullptr;

		initSceneGraphData(GT, pParent, Translation, Rotation, Scale);
	}//init

	void DatasetDisplayData::addToSceneGraph(CForge::ISceneGraphNode* pParent) {
		if (m_TransSGN.parent() != nullptr) m_TransSGN.parent()->removeChild(&m_TransSGN);
		pParent->addChild(&m_TransSGN);
	}//addToSceneGraph

	void DatasetDisplayData::removeFromSceneGraph(void) {
		if (m_TransSGN.parent() == nullptr) return; // nothing to do
		m_TransSGN.parent()->removeChild(&m_TransSGN);
	}//removeFromSceneGraph

	void DatasetDisplayData::clear(void) {
		m_PrimitivesGeomSGN.clear();
		m_WireframeGeomSGN.clear();
		m_TransSGN.clear();
		m_pPrimitivesActor->release();
		if (m_pWireframeActor != nullptr) m_pWireframeActor->release();
	}//clear

	DatasetShadingMode DatasetDisplayData::shadingMode(void) const {
		return m_ShadingMode;
	}//shadingMode
	
	void DatasetDisplayData::shadingMode(DatasetShadingMode Mode) {
		m_ShadingMode = Mode;
	}//shadingMode

	void DatasetDisplayData::show(bool Show) {
		m_TransSGN.enable(true, Show);
	}//show

	void DatasetDisplayData::showPrimitives(bool Show) {
		m_PrimitivesGeomSGN.enable(true, Show);
	}//showPrimitives

	void DatasetDisplayData::showWireframe(bool Show) {
		m_WireframeGeomSGN.enable(true, Show);
	}//showWireframe

	bool DatasetDisplayData::shown(void) {
		bool Enabled;
		m_TransSGN.enabled(nullptr, &Enabled);
		return Enabled;
	}//shown

	void DatasetDisplayData::primitivesColor(CForge::T3DMesh<float>* pModel, Vector3f Color) {
		std::vector<Vector3f> Colors;
		for (uint32_t i = 0; i < pModel->vertexCount(); ++i) Colors.push_back(Color);
		pModel->colors(&Colors);
		Colors.clear();

		DatasetRenderMode RM = m_pPrimitivesActor->renderMode();
		m_pPrimitivesActor->release();
		m_pPrimitivesActor = new DatasetActor();
		m_pPrimitivesActor->init(pModel, RM);
		m_PrimitivesGeomSGN.actor(m_pPrimitivesActor);
	}//primitivesColor

	void DatasetDisplayData::wireframeColor(CForge::T3DMesh<float>* pModel, Vector3f Color) {
		std::vector<Vector3f> Colors;
		for (uint32_t i = 0; i < pModel->vertexCount(); ++i) Colors.push_back(Color);
		pModel->colors(&Colors);
		Colors.clear();

		DatasetRenderMode RM = m_pWireframeActor->renderMode();
		m_pWireframeActor->release();
		m_pWireframeActor = new DatasetActor();
		m_pWireframeActor->init(pModel, RM);
		m_WireframeGeomSGN.actor(m_pWireframeActor);
	}//wireframeColor

	Quaternionf DatasetDisplayData::rotation(void) const {
		return m_TransSGN.rotation();
	}//rotation

	void DatasetDisplayData::rotation(Quaternionf Rotation) {
		m_TransSGN.rotation(Rotation);
	}//rotation

	Vector3f DatasetDisplayData::translation(void) const {
		return m_TransSGN.translation();
	}//translation

	void DatasetDisplayData::translation(Vector3f Translation) {
		m_TransSGN.translation(Translation);
	}//translation

	CForge::SGNTransformation* DatasetDisplayData::transformationSGN(void) {
		return &m_TransSGN;
	}//transformationSGN

	Matrix4f DatasetDisplayData::modelMatrix(void) {
		Vector3f Translation = Vector3f::Zero();
		Quaternionf Rotation = Quaternionf::Identity();
		Vector3f Scaling = Vector3f::Ones();

		m_TransSGN.buildTansformation(&Translation, &Rotation, &Scaling);

		Matrix4f TransMat = Matrix4f::Identity();
		TransMat(0, 3) = Translation(0);
		TransMat(1, 3) = Translation(1);
		TransMat(2, 3) = Translation(2);

		Matrix4f RotMat = Matrix4f::Identity();
		Matrix3f RotQuatToMat = Rotation.toRotationMatrix();
		RotMat.block<3, 3>(0, 0) = RotQuatToMat;

		Matrix4f ScaleMat = Matrix4f::Identity();
		ScaleMat(0, 0) = Scaling(0);
		ScaleMat(1, 1) = Scaling(1);
		ScaleMat(2, 2) = Scaling(2);

		Matrix4f Model = TransMat * RotMat * ScaleMat;
		return Model;
	}//modelMatrix

	void DatasetDisplayData::initPrimitivesActor(DatasetGeometryType GT, CForge::T3DMesh<float>* pModel, Vector3f Color) {
		m_pPrimitivesActor = new DatasetActor();
		DatasetRenderMode RM = (GT == DatasetGeometryType::MESH) ? DatasetRenderMode::FILL : DatasetRenderMode::POINT;

		std::vector<Vector3f> Colors;
		for (uint32_t i = 0; i < pModel->vertexCount(); ++i) Colors.push_back(Color);
		pModel->colors(&Colors);
		Colors.clear();

		m_pPrimitivesActor->init(pModel, RM);
	}//initPrimitivesActor

	void DatasetDisplayData::initWireframeActor(CForge::T3DMesh<float>* pModel, Vector3f Color) {
		m_pWireframeActor = new DatasetActor();

		std::vector<Vector3f> Colors;
		for (uint32_t i = 0; i < pModel->vertexCount(); ++i) Colors.push_back(Color);
		pModel->colors(&Colors);
		Colors.clear();

		m_pWireframeActor->init(pModel, DatasetRenderMode::LINE);
	}//initWireframeActor

	void DatasetDisplayData::initSceneGraphData(DatasetGeometryType GT, CForge::ISceneGraphNode* pParent, Eigen::Vector3f Translation, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale) {
		m_TransSGN.init(pParent, Translation, Rotation, Scale);
		m_TransSGN.enable(true, false);

		m_PrimitivesGeomSGN.init(&m_TransSGN, m_pPrimitivesActor);

		if (GT == DatasetGeometryType::MESH) {
			m_WireframeGeomSGN.init(&m_TransSGN, m_pWireframeActor);
			m_WireframeGeomSGN.enable(true, false);
		}
	}//initSceneGraphData
}