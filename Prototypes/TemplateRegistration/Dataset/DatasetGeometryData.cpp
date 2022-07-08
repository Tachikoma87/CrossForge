#include "DatasetGeometryData.h"

#include "../../../CForge/AssetIO/SAssetIO.h"
#include "../../../CForge/AssetIO/T3DMesh.hpp"

//#include <igl/readOBJ.h>
//#include <igl/readPLY.h>

namespace TempReg {

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
		for (int32_t i = 0; i < M.vertexCount(); ++i)
			m_Vertices.row(i) << M.vertex(i).x(), M.vertex(i).y(), M.vertex(i).z();

		if (GT == DatasetGeometryType::MESH) {
			auto S = M.getSubmesh(0);

			m_VertexNormals.resize(M.normalCount(), 3);
			for (int32_t i = 0; i < M.normalCount(); ++i)
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

	void DatasetGeometryData::initMeshAdjacencyData(void) {
		if (m_GeometryType == DatasetGeometryType::POINTCLOUD) throw CForgeExcept("Incompatible geometry type");

		// store faces adjacent to each vertex

		std::vector<std::vector<size_t>> TempVertAdjLists;
		TempVertAdjLists.resize(m_Vertices.rows());
		for (size_t i = 0; i < m_Vertices.rows(); ++i) TempVertAdjLists[i].reserve(8);

		for (size_t i = 0; i < m_Faces.rows(); ++i) {
			TempVertAdjLists[m_Faces(i, 0)].push_back(i);
			TempVertAdjLists[m_Faces(i, 1)].push_back(i);
			TempVertAdjLists[m_Faces(i, 2)].push_back(i);
		}

		m_VertexFaceAdjRefMap.resize(m_Vertices.rows());
		size_t FirstRef = 0;
		size_t LastRef = TempVertAdjLists[0].size() - 1;
		size_t TotalRefCount = 0;

		for (size_t i = 0; i < m_Vertices.rows(); ++i) {
			TotalRefCount += TempVertAdjLists[i].size();

			m_VertexFaceAdjRefMap[i].FirstRef = FirstRef;
			m_VertexFaceAdjRefMap[i].LastRef = LastRef;

			FirstRef = LastRef + 1;
			LastRef = FirstRef + TempVertAdjLists[(i + 1) % m_Vertices.rows()].size() - 1;
		}

		m_VertexFaceAdjacency.reserve(TotalRefCount);
		for (size_t i = 0; i < m_Vertices.rows(); ++i) {
			m_VertexFaceAdjacency.insert(m_VertexFaceAdjacency.end(), TempVertAdjLists[i].begin(), TempVertAdjLists[i].end());
			TempVertAdjLists[i].clear();
		}
		TempVertAdjLists.clear();

		// store faces adjacent to each face

		m_FaceFaceAdjacency.assign(m_Faces.rows() * 3, -1);

		for (size_t i = 0, FirstEdge = 0; i < m_Faces.rows(); ++i, FirstEdge += 3) {

			// check adjacency lists of each vertex in face i

			size_t V0 = m_Faces(i, 0);
			size_t V1 = m_Faces(i, 1);
			size_t V2 = m_Faces(i, 2);

			// edge V0V1 of face i:

			size_t V0FirstRef = m_VertexFaceAdjRefMap[V0].FirstRef;
			size_t V0LastRef = m_VertexFaceAdjRefMap[V0].LastRef;

			// check if any of the faces adjacent to V0 (excluding the currently processed face i itself) also contain V1 of face i; if so, both faces are adjacent to each other
			for (size_t Ref = V0FirstRef; Ref <= V0LastRef; ++Ref) {
				size_t AdjFace = m_VertexFaceAdjacency[Ref];

				if (AdjFace == i) continue;

				if (m_Faces(AdjFace, 0) == V1 || m_Faces(AdjFace, 1) == V1 || m_Faces(AdjFace, 2) == V1) {
					m_FaceFaceAdjacency[FirstEdge] = AdjFace;
					break;
				}
			}

			// edge V1V2 of face i:

			size_t V1FirstRef = m_VertexFaceAdjRefMap[V1].FirstRef;
			size_t V1LastRef = m_VertexFaceAdjRefMap[V1].LastRef;

			// check if any of the faces adjacent to V1 (excluding the currently processed face i itself) also contain V2 of face i; if so, both faces are adjacent to each other
			for (size_t Ref = V1FirstRef; Ref <= V1LastRef; ++Ref) {
				size_t AdjFace = m_VertexFaceAdjacency[Ref];

				if (AdjFace == i) continue;

				if (m_Faces(AdjFace, 0) == V2 || m_Faces(AdjFace, 1) == V2 || m_Faces(AdjFace, 2) == V2) {
					m_FaceFaceAdjacency[FirstEdge + 1] = AdjFace;
					break;
				}
			}

			// edge V2V0 of face i:

			size_t V2FirstRef = m_VertexFaceAdjRefMap[V2].FirstRef;
			size_t V2LastRef = m_VertexFaceAdjRefMap[V2].LastRef;

			// check if any of the faces adjacent to V2 (excluding the currently processed face i itself) also contain V0 of face i; if so, both faces are adjacent to each other
			for (size_t Ref = V2FirstRef; Ref <= V2LastRef; ++Ref) {
				size_t AdjFace = m_VertexFaceAdjacency[Ref];

				if (AdjFace == i) continue;

				if (m_Faces(AdjFace, 0) == V0 || m_Faces(AdjFace, 1) == V0 || m_Faces(AdjFace, 2) == V0) {
					m_FaceFaceAdjacency[FirstEdge + 2] = AdjFace;
					break;
				}
			}
		}
	}//initMeshAdjacencyData

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
		return m_Vertices.rows();
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
		return m_Faces.rows();
	}//faceCount

	int64_t DatasetGeometryData::faceAdjacentToEdge(size_t Face, size_t EdgeBegin) const {
		if (Face > m_Faces.rows()) throw IndexOutOfBoundsExcept("Face");
		if (EdgeBegin > 2) throw CForgeExcept("EdgeBegin must be in range [0, 2]");
		return m_FaceFaceAdjacency[Face * 3 + EdgeBegin];
	}//faceAdjacentToEdge

	void DatasetGeometryData::facesAdjacentToFace(size_t OriginFace, std::vector<size_t>& AdjacentFaces, int64_t PrevOriginFace, int64_t AdjacencyLayers) const {
		if (OriginFace > m_Faces.rows()) throw IndexOutOfBoundsExcept("OriginFace");
		if (PrevOriginFace > m_Faces.rows()) throw IndexOutOfBoundsExcept("PrevOriginFace");
		if (AdjacencyLayers < 1) return;

		std::vector<size_t> AdjFaces;
		for (size_t i = 0; i < 3; ++i) {
			const auto& AdjFace = m_FaceFaceAdjacency[OriginFace * 3 + i];
			if (AdjFace > -1 && AdjFace != PrevOriginFace) AdjFaces.push_back(m_FaceFaceAdjacency[OriginFace * 3 + i]);
		}

		AdjacentFaces.insert(AdjacentFaces.end(), AdjFaces.begin(), AdjFaces.end());

		for (auto AdjFace : AdjFaces) facesAdjacentToFace(AdjFace, AdjacentFaces, OriginFace, AdjacencyLayers - 1);
	}//facesAdjacentToFace

	std::vector<size_t> DatasetGeometryData::facesAdjacentToVertex(size_t Vertex) const {
		if (Vertex > m_Vertices.rows()) throw IndexOutOfBoundsExcept("Vertex");

		std::vector<size_t> AdjacentFaces;
		const auto& RefRange = m_VertexFaceAdjRefMap[Vertex];
		AdjacentFaces.insert(AdjacentFaces.end(), m_VertexFaceAdjacency.begin() + RefRange.FirstRef, m_VertexFaceAdjacency.begin() + (RefRange.LastRef + 1));
		return AdjacentFaces;
	}//facesAdjacentToVertex

	const igl::embree::EmbreeIntersector& DatasetGeometryData::embreeIntersector(void) const {
		return m_EI;
	}//embreeIntersector
}