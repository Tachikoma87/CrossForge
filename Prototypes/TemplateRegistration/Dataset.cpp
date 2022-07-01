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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Correspondence Data
	// 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CorrespondenceStorage::CorrespondenceStorage(size_t TemplateVertexCount, size_t TargetVertexCount) {
		m_TemplateData.VertexCount = TemplateVertexCount;
		m_TargetData.VertexCount = TargetVertexCount;
	}//Constructor

	CorrespondenceStorage::~CorrespondenceStorage() {

	}//Destructor

	void CorrespondenceStorage::addCorrespondence(CorrespondenceType CT, size_t TemplateVertexID, size_t TargetVertexID) {
		if (CT == CorrespondenceType::NONE) throw CForgeExcept("Invalid value for CT");
		if (m_TemplateData.VertexData.count(TemplateVertexID) > 0) throw CForgeExcept("Template vertex already in use");

		// add template correspondence data

		auto TemplateVertex = m_TemplateData.VertexData.try_emplace(TemplateVertexID);
		if (!TemplateVertex.second) throw CForgeExcept("Insertion of template vertex failed");

		TemplateVertex.first->second.TargetPoint = TargetVertexID;
		TemplateVertex.first->second.TargetPointIsVertex = true;
		TemplateVertex.first->second.Type = CT;

		// add target correspondence data
	
		if (m_TargetData.VertexData.count(TemplateVertexID) == 0) {
			auto TargetVertex = m_TargetData.VertexData.try_emplace(TargetVertexID);
			if (!TargetVertex.second) throw CForgeExcept("Insertion of target vertex failed");
		}
		m_TargetData.VertexData.at(TargetVertexID).push_back(TemplateVertexID);

		if (CT == CorrespondenceType::CORRESPONDENCE) m_TemplateData.CorrQuickAccess.insert(TemplateVertexID);
		else m_TemplateData.FeatQuickAccess.insert(TemplateVertexID);
	}//addCorrespondence
	
	void CorrespondenceStorage::addCorrespondence(CorrespondenceType CT, size_t TemplateVertexID, DatasetGeometryData& TargetGeometry, size_t TargetFace, Vector3f TargetPoint, Vector3f TargetBaryCoords) {
		if (CT == CorrespondenceType::NONE) throw CForgeExcept("Invalid value for CT");
		if (m_TemplateData.VertexData.count(TemplateVertexID) > 0) throw CForgeExcept("Template vertex already in use");

		// add correspondence data for new target correspondence point

		// -> test if TargetPoint is new or existing correspondence point
		int64_t TargetPointID = -1;
		const auto& ExistingPoints = m_TargetData.FaceToSurfacePoints.at(TargetFace);
		if (!ExistingPoints.empty()) {
			for (auto Point : ExistingPoints) {
				float Dist = (m_TargetData.SurfacePointData[Point].Position - TargetPoint).squaredNorm();
				if (Dist < (float)1e-6) {
					TargetPointID = Point;
					break;
				}
			}
		}

		if (TargetPointID < 0) {
			// -> get ID for new unique surface point
			if (!m_TargetData.FreeSurfacePoints.empty()) {
				TargetPointID = *m_TargetData.FreeSurfacePoints.begin();
				m_TargetData.FreeSurfacePoints.erase(m_TargetData.FreeSurfacePoints.begin());
			}
			else {
				TargetPointID = m_TargetData.SurfacePointData.size();
				m_TargetData.SurfacePointData.push_back(TargetSurfacePointData());
			}
		}
		auto& TargetSurfacePoint = m_TargetData.SurfacePointData[TargetPointID];
		TargetSurfacePoint.Position = TargetPoint;
		TargetSurfacePoint.TemplateVertices.push_back(TemplateVertexID);
		TargetSurfacePoint.TargetFaces.push_back(TargetFace);

		auto FaceMapping = m_TargetData.FaceToSurfacePoints.try_emplace(TargetFace);
		if (!FaceMapping.second) throw CForgeExcept("Insertion of face-to-surface point mapping failed");
		FaceMapping.first->second.push_back(TargetPointID);

		// -> check if target correspondence point touches any edges of TargetFace; add references to adjacent faces of touched edges
		std::array<bool, 3> TouchedEdges = surfacePointOnEdge(TargetPoint, TargetBaryCoords);

		for (size_t i = 0; i < 3; ++i) {
			if (TouchedEdges[i]) { // edge i begins at vertex i
				auto AdjFace = TargetGeometry.faceAdjacentToEdge(TargetFace, i);
				if (AdjFace != -1) {
					if (m_TargetData.FaceToSurfacePoints.count(AdjFace) == 0) {
						auto AdjFaceMapping = m_TargetData.FaceToSurfacePoints.try_emplace(AdjFace);
						if (!AdjFaceMapping.second) throw CForgeExcept("Insertion of face-to-surface point mapping for adjacent face failed");
					}
					m_TargetData.FaceToSurfacePoints.at(AdjFace).push_back(TargetPointID);
					TargetSurfacePoint.TargetFaces.push_back(AdjFace);
				}
			}
		}

		// add correspondence data for template

		auto TemplateVertex = m_TemplateData.VertexData.try_emplace(TemplateVertexID);
		if (!TemplateVertex.second) throw CForgeExcept("Insertion of template vertex failed");

		TemplateVertex.first->second.TargetPoint = TargetPointID;
		TemplateVertex.first->second.TargetPointIsVertex = false;
		TemplateVertex.first->second.Type = CT;

		if (CT == CorrespondenceType::CORRESPONDENCE) m_TemplateData.CorrQuickAccess.insert(TemplateVertexID);
		else m_TemplateData.FeatQuickAccess.insert(TemplateVertexID);
	}//addCorrespondence
		
	void CorrespondenceStorage::removeCorrespondence(size_t TemplateVertexID) {
		auto TemplateVertex = m_TemplateData.VertexData.find(TemplateVertexID);
		if (TemplateVertex == m_TemplateData.VertexData.end()) throw CForgeExcept("Template vertex not found");

		if (TemplateVertex->second.TargetPointIsVertex) eraseVertexVertexPair(TemplateVertexID, true);
		else eraseVertexSurfacePointPair(TemplateVertexID, true);
	}//removeCorrespondence

	void CorrespondenceStorage::clearCorrespondences(CorrespondenceType CT) {
		if (CT == CorrespondenceType::NONE) {
			m_TemplateData.VertexData.clear();
			m_TemplateData.CorrQuickAccess.clear();
			m_TemplateData.FeatQuickAccess.clear();

			for (auto& Vertex : m_TargetData.VertexData) Vertex.second.clear();
			m_TargetData.VertexData.clear();

			m_TargetData.SurfacePointData.clear();
			m_TargetData.FreeSurfacePoints.clear();

			for (auto& FaceMapping : m_TargetData.FaceToSurfacePoints) FaceMapping.second.clear();
			m_TargetData.FaceToSurfacePoints.clear();

			return;
		}

		auto& TemplatePoints = (CT == CorrespondenceType::CORRESPONDENCE) ? m_TemplateData.CorrQuickAccess : m_TemplateData.FeatQuickAccess;
		for (auto P : TemplatePoints) {
			if (m_TemplateData.VertexData.at(P).TargetPointIsVertex) eraseVertexVertexPair(P, false);
			else eraseVertexSurfacePointPair(P, false);
		}
		TemplatePoints.clear();
	}//clearCorrespondences

	bool CorrespondenceStorage::templateVertexAvailable(size_t TemplateVertexID) const {
		if (m_TemplateData.VertexData.count(TemplateVertexID) == 0) return true;
		return false;
	}//templateVertexAvailable

	bool CorrespondenceStorage::correspondenceAtTemplateVertex(CorrespondencePair& Result, size_t Vertex) const {
		const auto TemplateVertex = m_TemplateData.VertexData.find(Vertex);
		if (TemplateVertex == m_TemplateData.VertexData.end()) return false;
		
		Result.TemplatePointUID = Vertex;
		Result.TargetPoint.UID = externalTargetID(Vertex);
		Result.TargetPoint.IsVertex = TemplateVertex->second.TargetPointIsVertex;
		Result.Type = TemplateVertex->second.Type;
		
		if (!TemplateVertex->second.TargetPointIsVertex) {
			const auto& TargetSurfacePoint = m_TargetData.SurfacePointData[TemplateVertex->second.TargetPoint];
			Result.TargetPoint.Position = TargetSurfacePoint.Position;
		}

		return true;
	}//correspondenceAtTemplateVertex
	
	bool CorrespondenceStorage::correspondencesAtTargetVertex(std::vector<CorrespondencePair>& Result, size_t Vertex) const {
		const auto TargetVertex = m_TargetData.VertexData.find(Vertex);
		if (TargetVertex == m_TargetData.VertexData.end()) return false;
		
		Result.resize(TargetVertex->second.size());

		for (int i = 0; i < Result.size(); ++i) {
			auto TemplateVertexID = TargetVertex->second[i];
			const auto& TemplateVertex = m_TemplateData.VertexData.at(TemplateVertexID);
			
			Result[i].TemplatePointUID = TemplateVertexID;
			Result[i].TargetPoint.UID = externalTargetID(Vertex);
			Result[i].TargetPoint.IsVertex = true;
			Result[i].Type = TemplateVertex.Type;
		}

		return true;
	}//correspondencesAtTargetVertex

	bool CorrespondenceStorage::correspondencesAtTargetMeshPoint(std::vector<CorrespondencePair>& Result, size_t TargetFace, Vector3f SelectionPoint, float Tolerance) const {
		const auto& ExistingPoints = m_TargetData.FaceToSurfacePoints.at(TargetFace);
		if (ExistingPoints.empty()) return false;

		int64_t TargetPointID = -1;
		for (auto Point : ExistingPoints) {
			float Dist = (m_TargetData.SurfacePointData[Point].Position - SelectionPoint).squaredNorm();
			if (Dist < (float)1e-6) {
				TargetPointID = Point;
				break;
			}
		}

		if (TargetPointID == -1) return false;

		const auto& TargetSurfacePoint = m_TargetData.SurfacePointData[TargetPointID];
		if (TargetSurfacePoint.TemplateVertices.empty()) throw CForgeExcept("Target correspondence point is missing template vertices");

		size_t ExternalTargetPointID = externalTargetID(TargetPointID);
		Result.resize(TargetSurfacePoint.TemplateVertices.size());

		for (int i = 0; i < Result.size(); ++i) {
			auto TemplateVertexID = TargetSurfacePoint.TemplateVertices[i];
			const auto& TemplateVertex = m_TemplateData.VertexData.at(TemplateVertexID);
						
			Result[i].TemplatePointUID = TemplateVertexID;
			Result[i].TargetPoint.UID = ExternalTargetPointID;
			Result[i].TargetPoint.IsVertex = false;
			Result[i].TargetPoint.Position = TargetSurfacePoint.Position;
			Result[i].Type = TemplateVertex.Type;
		}

		return true;
	}//correspondencesAtTargetMeshPoint
	
	const std::set<size_t>& CorrespondenceStorage::correspondencesQuickAccess(void) const {
		return m_TemplateData.CorrQuickAccess;
	}//correspondencesQuickAccess

	const std::set<size_t>& CorrespondenceStorage::featuresQuickAccess(void) const {
		return m_TemplateData.FeatQuickAccess;
	}//featuresQuickAccess

	void CorrespondenceStorage::eraseVertexVertexPair(size_t TemplateVertexID, bool EraseQuickAccess) {
		// remove target correspondence data

		size_t TargetPoint = m_TemplateData.VertexData.at(TemplateVertexID).TargetPoint;
		
		m_TargetData.VertexData.at(TargetPoint).erase(m_TargetData.VertexData.at(TargetPoint).begin() + TemplateVertexID); // remove reference to template correspondence point

		if (m_TargetData.VertexData.at(TargetPoint).empty()) // remove unique target correspondence point if no longer used by any correspondence pairs
			m_TargetData.VertexData.erase(TargetPoint); 
		
		// remove template correspondence data

		if (EraseQuickAccess) {
			auto CorrType = m_TemplateData.VertexData.at(TemplateVertexID).Type;
			if (CorrType == CorrespondenceType::CORRESPONDENCE) m_TemplateData.CorrQuickAccess.erase(TemplateVertexID);
			else m_TemplateData.FeatQuickAccess.erase(TemplateVertexID);
		}
		
		m_TemplateData.VertexData.erase(TemplateVertexID);
	}//eraseVertexVertexPair
	
	void CorrespondenceStorage::eraseVertexSurfacePointPair(size_t TemplateVertexID, bool EraseQuickAccess) {
		// remove target correspondence data

		size_t TargetPoint = m_TemplateData.VertexData.at(TemplateVertexID).TargetPoint;
		auto& TargetSurfacePoint = m_TargetData.SurfacePointData[TargetPoint];

		TargetSurfacePoint.TemplateVertices.erase(TargetSurfacePoint.TemplateVertices.begin() + TemplateVertexID); // remove reference to template correspondence point

		if (TargetSurfacePoint.TemplateVertices.empty()) { // remove unique target correspondence point if no longer used by any correspondence pairs
			for (auto Face : TargetSurfacePoint.TargetFaces) {
				auto& FaceMapping = m_TargetData.FaceToSurfacePoints.at(Face);
				FaceMapping.erase(FaceMapping.begin() + TargetPoint);
				if (FaceMapping.empty()) m_TargetData.FaceToSurfacePoints.erase(Face);
			}

			TargetSurfacePoint.Position = Vector3f::Zero();
			TargetSurfacePoint.TargetFaces.clear();
			TargetSurfacePoint.TemplateVertices.clear();

			m_TargetData.FreeSurfacePoints.insert(TargetPoint);
		}
		
		// remove template correspondence data

		if (EraseQuickAccess) {
			auto CorrType = m_TemplateData.VertexData.at(TemplateVertexID).Type;
			if (CorrType == CorrespondenceType::CORRESPONDENCE) m_TemplateData.CorrQuickAccess.erase(TemplateVertexID);
			else m_TemplateData.FeatQuickAccess.erase(TemplateVertexID);
		}
		
		m_TemplateData.VertexData.erase(TemplateVertexID);
	}//eraseVertexSurfacePointPair

	std::array<bool, 3> CorrespondenceStorage::surfacePointOnEdge(Vector3f TargetPosition, Vector3f BaryCoords) {
		std::array<bool, 3> TouchedEdges = { false, false, false };
		if (BaryCoords.x() < (float)1e-6) TouchedEdges[1] = true;
		if (BaryCoords.y() < (float)1e-6) TouchedEdges[2] = true;
		if (BaryCoords.z() < (float)1e-6) TouchedEdges[0] = true;
		return TouchedEdges;
	}//surfacePointOnEdge

	size_t CorrespondenceStorage::externalTargetID(size_t TemplateVertexID) const {
		const auto& TemplateVertex = m_TemplateData.VertexData.at(TemplateVertexID);
		size_t ExID = (TemplateVertex.TargetPointIsVertex) ? TemplateVertex.TargetPoint : TemplateVertex.TargetPoint + m_TargetData.VertexCount;
		return ExID;
	}//externalTargetCorrID

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
		case DatasetRenderMode::LINE: {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
			glLineWidth(2);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1, -1);
			break;
		}
		case DatasetRenderMode::POINT: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
		default: break;
		}
		
		m_Actor.render(pRDev);

		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(0, 0);
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}//render

	void DatasetActor::renderMode(DatasetRenderMode RM) {
		m_RenderMode = RM;
	}//renderMode

	DatasetRenderMode DatasetActor::renderMode(void)const {
		return m_RenderMode;
	}//renderMode
}