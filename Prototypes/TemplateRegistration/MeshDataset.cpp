#include "MeshDataset.h"

#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../Examples/SceneUtilities.hpp"

namespace TempReg {

	MeshDataset::MeshDataset(void) : 
		m_VisualizationMode(DatasetVisualizationMode::SOLID_COLOR) {

	}//Constructor
	
	MeshDataset::~MeshDataset() {
		m_RawMesh.clear();
	}//Destructor

	void MeshDataset::initFromFile(std::string Filepath, bool ComputeNormals, Vector3f DefaultColor) {
		CForge::SAssetIO::load(Filepath, &m_RawMesh);
		CForge::SceneUtilities::setMeshShader(&m_RawMesh, 0.4f, 0.0f);

		if (ComputeNormals) m_RawMesh.computePerVertexNormals();

		std::vector<Vector3f> Colors;
		for (uint32_t i = 0; i < m_RawMesh.vertexCount(); i++) Colors.push_back(DefaultColor);
		m_RawMesh.colors(&Colors);
		Colors.clear();

		m_RawMesh.computeAxisAlignedBoundingBox();

		// TODO: BVH

		// TODO: mathematical representations?
	}//initFromFile

	void MeshDataset::initFromMeshDataset(MeshDataset& Mesh) {//TODO
		//...
	}//initFromMeshDataset

	void MeshDataset::visualizationMode(DatasetVisualizationMode Mode) {
		m_VisualizationMode = Mode;
	}//visualizationMode

	const CForge::T3DMesh<float>* MeshDataset::mesh(void) {
		return &m_RawMesh;
	}//mesh
}