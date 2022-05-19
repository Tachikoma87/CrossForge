#include <glad/glad.h>

#include "Dataset.h"

#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../Examples/SceneUtilities.hpp"

#include <igl/readOBJ.h>
#include <igl/readPLY.h>

namespace TempReg {

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Geometry Data
	// 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	DatasetGeometryData::DatasetGeometryData(void) {

	}//Constructor
	
	DatasetGeometryData::~DatasetGeometryData() {
		
	}//Destructor

	void DatasetGeometryData::initFromFile(std::string Filepath, DatasetGeometryType GT) { //TODO
		m_GeometryType = GT;

		CForge::T3DMesh<float> M;
		CForge::SAssetIO::load(Filepath, &M);
		M.computePerVertexNormals();
		
		m_Vertices.resize(M.vertexCount(), 3);
		for (uint32_t i = 0; i < M.vertexCount(); ++i)
			m_Vertices.row(i) << M.vertex(i).x(), M.vertex(i).y(), M.vertex(i).z();

		if (GT == DatasetGeometryType::MESH) {
			auto S = M.getSubmesh(0);

			m_VertexNormals.resize(M.normalCount(), 3);
			for (uint32_t i = 0; i < M.normalCount(); ++i)
				m_VertexNormals.row(i) << M.normal(i).x(), M.normal(i).y(), M.normal(i).z();
			
			m_Faces.resize(S->Faces.size(), 3);
			for (size_t i = 0; i < S->Faces.size(); ++i)
				m_Faces.row(i) << S->Faces[i].Vertices[0], S->Faces[i].Vertices[1], S->Faces[i].Vertices[2];
		}

		M.clear();

		//if (Filepath.find(".obj") != std::string::npos) igl::readOBJ(Filepath, m_Vertices, m_Faces); // test
		//if (Filepath.find(".ply") != std::string::npos) igl::readPLY(Filepath, m_Vertices, m_Faces); // test

		if (GT == DatasetGeometryType::MESH) {
			initEmbreeIntersector();
			//initMeshClpBVH();
		}
		else {
			//initPclBVH();
		}
	}//initFromFile

	void DatasetGeometryData::initEmbreeIntersector(void) {
		m_EI.init(m_Vertices.cast<float>(), m_Faces);
	}//initEmbreeIntersector
	
	/*void DatasetGeometryData::initMeshClpBVH(void) { //TODO

	}//initMeshClpBVH*/
	
	/*void DatasetGeometryData::initPclBVH(void) { //TODO

	}//initPclBVH*/

	void DatasetGeometryData::updateActiveBVHs(void) { //TODO
		if (m_GeometryType == DatasetGeometryType::MESH) {
			m_EI.deinit();
			m_EI.init(m_Vertices.cast<float>(), m_Faces);

			//TODO: update MeshClpBVH
		}
		else {
			//TODO: update PclBVH
		}
	}//updateBVH

	DatasetGeometryType DatasetGeometryData::geometryType(void) const {
		return m_GeometryType;
	}//geometryType

	const Vector3f DatasetGeometryData::vertex(Index Idx) const {
		Vector3f Vertex;
		Vertex(0) = (float)m_Vertices(Idx, 0);
		Vertex(1) = (float)m_Vertices(Idx, 1);
		Vertex(2) = (float)m_Vertices(Idx, 2);
		return Vertex;
	}//vertex

	const MatrixXd& DatasetGeometryData::vertices(void) const {
		return m_Vertices;
	}//vertices

	const MatrixXd DatasetGeometryData::faceVertices(Index FaceIdx) const {
		MatrixXd Vertices;
		Vertices.resize(3, 3);

		Vertices(0, 0) = m_Vertices(m_Faces(FaceIdx, 0), 0);
		Vertices(0, 1) = m_Vertices(m_Faces(FaceIdx, 0), 1);
		Vertices(0, 2) = m_Vertices(m_Faces(FaceIdx, 0), 2);

		Vertices(1, 0) = m_Vertices(m_Faces(FaceIdx, 1), 0);
		Vertices(1, 1) = m_Vertices(m_Faces(FaceIdx, 1), 1);
		Vertices(1, 2) = m_Vertices(m_Faces(FaceIdx, 1), 2);

		Vertices(2, 0) = m_Vertices(m_Faces(FaceIdx, 2), 0);
		Vertices(2, 1) = m_Vertices(m_Faces(FaceIdx, 2), 1);
		Vertices(2, 2) = m_Vertices(m_Faces(FaceIdx, 2), 2);

		return Vertices;
	}//faceVertices

	size_t DatasetGeometryData::vertexCount(void) const {
		return m_Vertices.rows(); // Dummy Variable
	}//vertexCount

	const Vector3i DatasetGeometryData::face(Index Idx) const {
		Vector3i Face;
		Face(0) = m_Faces(Idx, 0);
		Face(1) = m_Faces(Idx, 1);
		Face(2) = m_Faces(Idx, 2);
		return Face;
	}//face

	const MatrixXi& DatasetGeometryData::faces(void) const {
		return m_Faces;
	}//faces

	size_t DatasetGeometryData::faceCount(void) const {
		return m_Faces.rows(); // Dummy Variable
	}//faceCount

	const igl::embree::EmbreeIntersector& DatasetGeometryData::embreeIntersector(void) const {
		return m_EI;
	}//embreeIntersector

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Render Data
	// 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	DatasetActor::DatasetActor(void) : CForge::IRenderableActor("DatasetActor", 10) {

	}//Constructor

	DatasetActor::~DatasetActor(void) {
		m_Actor.clear();
	}//Destructor

	void DatasetActor::init(const CForge::T3DMesh<float>* pMesh, DatasetRenderMode RM) {
		m_Actor.init(pMesh);
		m_RenderMode = RM;
	}//initialize

	void DatasetActor::release(void) {
		delete this;
	}//release

	void DatasetActor::render(CForge::RenderDevice* pRDev) {
		switch (m_RenderMode) {
		case DatasetRenderMode::FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		case DatasetRenderMode::LINE: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case DatasetRenderMode::POINT: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
		default: break;
		}

		m_Actor.render(pRDev);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}//render

	void DatasetActor::renderMode(DatasetRenderMode RM) {
		m_RenderMode = RM;
	}//renderMode

	DatasetRenderMode DatasetActor::renderMode(void)const {
		return m_RenderMode;
	}//renderMode
}