#include "CorrespondenceStorage.h"

#include "../../../CForge/Core/CrossForgeException.h"

#include "DatasetGeometryData.h"

namespace TempReg {

	CorrespondenceStorage::CorrespondenceStorage(size_t TemplateVertexCount, size_t TargetVertexCount) {
		m_TemplateData.VertexData.resize(TemplateVertexCount);
		m_TargetData.VertexCount = TargetVertexCount;
	}//Constructor

	CorrespondenceStorage::~CorrespondenceStorage() {

	}//Destructor

	void CorrespondenceStorage::addCorrespondence(CorrespondenceType CT, size_t TemplateVertexID, size_t TargetVertexID) {
		if (CT == CorrespondenceType::NONE) throw CForgeExcept("Invalid value for CT");
		if (m_TemplateData.VertexData[TemplateVertexID].Type != CorrespondenceType::NONE) throw CForgeExcept("Template vertex already in use");

		// add template correspondence data

		m_TemplateData.VertexData[TemplateVertexID].TargetPoint = TargetVertexID;
		m_TemplateData.VertexData[TemplateVertexID].TargetPointIsVertex = true;
		m_TemplateData.VertexData[TemplateVertexID].Type = CT;

		// add target correspondence data

		if (m_TargetData.VertexData.count(TargetVertexID) == 0) {
			auto TargetVertex = m_TargetData.VertexData.try_emplace(TargetVertexID);
			if (!TargetVertex.second) throw CForgeExcept("Insertion of target vertex failed");
		}
		m_TargetData.VertexData.at(TargetVertexID).push_back(TemplateVertexID);

		if (CT == CorrespondenceType::CORRESPONDENCE) m_TemplateData.CorrQuickAccess.insert(TemplateVertexID);
		else m_TemplateData.FeatQuickAccess.insert(TemplateVertexID);
	}//addCorrespondence

	void CorrespondenceStorage::addCorrespondence(CorrespondenceType CT, size_t TemplateVertexID, DatasetGeometryData& TargetGeometry, size_t TargetFace, Vector3f TargetPoint, Vector3f TargetBaryCoords) {
		if (CT == CorrespondenceType::NONE) throw CForgeExcept("Invalid value for CT");
		if (m_TemplateData.VertexData[TemplateVertexID].Type != CorrespondenceType::NONE) throw CForgeExcept("Template vertex already in use");

		// add correspondence data for new target correspondence point

		// -> test if TargetPoint is new or existing surface point
		int64_t TargetPointID = -1;
		auto ExistingFaceMapping = m_TargetData.FaceToSurfacePoints.find(TargetFace);
		if (ExistingFaceMapping != m_TargetData.FaceToSurfacePoints.end()) {
			for (auto Point : ExistingFaceMapping->second) {
				float Dist = (m_TargetData.SurfacePointData[Point].Position - TargetPoint).squaredNorm();
				if (Dist < (float)1e-4) {
					TargetPointID = Point; // TargetPoint is existing surface point
					break;
				}
			}
		}
		
		// -> create new unique surface point if no existing surface point fits TargetPoint
		bool NewTargetPoint = false;
		if (TargetPointID < 0) {
			NewTargetPoint = true;
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
		TargetSurfacePoint.TemplateVertices.push_back(TemplateVertexID);
		
		if (NewTargetPoint) {
			TargetSurfacePoint.Position = TargetPoint;
			TargetSurfacePoint.TargetFaces.push_back(TargetFace);

			auto FaceMapping = m_TargetData.FaceToSurfacePoints.insert(std::pair<size_t, std::vector<size_t>>(TargetFace, std::vector<size_t>()));
			FaceMapping.first->second.push_back(TargetPointID);

			// -> check if target correspondence point touches any edges of TargetFace; add references to adjacent faces of touched edges
			std::array<bool, 3> CloseEdges = surfacePointNearEdge(TargetPoint, TargetBaryCoords);

			for (size_t i = 0; i < 3; ++i) {
				if (!CloseEdges[i]) continue; // edge i begins at vertex i

				auto AdjFace = TargetGeometry.faceAdjacentToEdge(TargetFace, i);

				if (AdjFace == -1) continue;
								
				auto AdjFaceMapping = m_TargetData.FaceToSurfacePoints.insert(std::pair<size_t, std::vector<size_t>>(AdjFace, std::vector<size_t>()));
				AdjFaceMapping.first->second.push_back(TargetPointID);
				TargetSurfacePoint.TargetFaces.push_back(AdjFace);
			}
		}
		// add correspondence data for template

		m_TemplateData.VertexData[TemplateVertexID].TargetPoint = TargetPointID;
		m_TemplateData.VertexData[TemplateVertexID].TargetPointIsVertex = false;
		m_TemplateData.VertexData[TemplateVertexID].Type = CT;

		if (CT == CorrespondenceType::CORRESPONDENCE) m_TemplateData.CorrQuickAccess.insert(TemplateVertexID);
		else m_TemplateData.FeatQuickAccess.insert(TemplateVertexID);
	}//addCorrespondence

	void CorrespondenceStorage::removeCorrespondence(size_t TemplateVertexID, std::vector<size_t>* RemainingTargetPartners) {
		if (m_TemplateData.VertexData[TemplateVertexID].Type == CorrespondenceType::NONE) throw CForgeExcept("Template vertex not found");
		if (m_TemplateData.VertexData[TemplateVertexID].TargetPointIsVertex) eraseVertexVertexPair(TemplateVertexID, true, RemainingTargetPartners);
		else eraseVertexSurfacePointPair(TemplateVertexID, true, RemainingTargetPartners);
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
			if (m_TemplateData.VertexData[P].TargetPointIsVertex) eraseVertexVertexPair(P, false, nullptr);
			else eraseVertexSurfacePointPair(P, false, nullptr);
		}
		TemplatePoints.clear();
	}//clearCorrespondences

	bool CorrespondenceStorage::templateVertexAvailable(size_t TemplateVertexID) const {
		if (m_TemplateData.VertexData[TemplateVertexID].Type == CorrespondenceType::NONE) return true;
		return false;
	}//templateVertexAvailable

	bool CorrespondenceStorage::correspondenceAtTemplateVertex(Correspondence& Result, size_t Vertex) const {
		if (m_TemplateData.VertexData[Vertex].Type == CorrespondenceType::NONE) return false;
		
		Result.TemplatePointUID = Vertex;
		Result.TargetPointUID = externalTargetID(m_TemplateData.VertexData[Vertex].TargetPoint, m_TemplateData.VertexData[Vertex].TargetPointIsVertex);
		//Result.TargetPointPos = (m_TemplateData.VertexData[Vertex].TargetPointIsVertex) ? Vector3f::Zero() : m_TargetData.SurfacePointData[m_TemplateData.VertexData[Vertex].TargetPoint].Position;
		Result.VertexVertexPair = m_TemplateData.VertexData[Vertex].TargetPointIsVertex;
		Result.Type = m_TemplateData.VertexData[Vertex].Type;

		return true;
	}//correspondenceAtTemplateVertex

	bool CorrespondenceStorage::correspondencesAtTargetVertex(std::vector<Correspondence>& Result, size_t Vertex) const {
		const auto TargetVertex = m_TargetData.VertexData.find(Vertex);
		if (TargetVertex == m_TargetData.VertexData.end()) return false;

		Result.resize(TargetVertex->second.size());

		for (int i = 0; i < Result.size(); ++i) {
			auto TemplateVertexID = TargetVertex->second[i];
			const auto& TemplateVertex = m_TemplateData.VertexData[TemplateVertexID];

			Result[i].TemplatePointUID = TemplateVertexID;
			Result[i].TargetPointUID = externalTargetID(TemplateVertex.TargetPoint, TemplateVertex.TargetPointIsVertex);
			//Result[i].TargetPointPos = Vector3f::Zero();
			Result[i].VertexVertexPair = true;
			Result[i].Type = TemplateVertex.Type;
		}

		return true;
	}//correspondencesAtTargetVertex

	bool CorrespondenceStorage::correspondencesAtTargetMeshPoint(std::vector<Correspondence>& Result, size_t TargetFace, Vector3f SelectionPoint, float Tolerance) const {
		auto ExistingFaceMapping = m_TargetData.FaceToSurfacePoints.find(TargetFace);
		if (ExistingFaceMapping == m_TargetData.FaceToSurfacePoints.end()) return false;

		int64_t TargetPointID = -1;
		for (auto Point : ExistingFaceMapping->second) {
			float Dist = (m_TargetData.SurfacePointData[Point].Position - SelectionPoint).squaredNorm();
			if (Dist < (float)1e-4) {
				TargetPointID = Point;
				break;
			}
		}

		if (TargetPointID == -1) return false;

		const auto& TargetSurfacePoint = m_TargetData.SurfacePointData[TargetPointID];
		if (TargetSurfacePoint.TemplateVertices.empty()) throw CForgeExcept("Target correspondence point is missing template vertices");

		size_t ExternalTargetPointID = externalTargetID(TargetPointID, true);
		Result.resize(TargetSurfacePoint.TemplateVertices.size());

		for (int i = 0; i < Result.size(); ++i) {
			auto TemplateVertexID = TargetSurfacePoint.TemplateVertices[i];
			const auto& TemplateVertex = m_TemplateData.VertexData[TemplateVertexID];

			Result[i].TemplatePointUID = TemplateVertexID;
			Result[i].TargetPointUID = ExternalTargetPointID;
			//Result[i].TargetPointPos = TargetSurfacePoint.Position;
			Result[i].VertexVertexPair = false;
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

	Vector3f CorrespondenceStorage::targetSurfacePointPosition(size_t PointID) const {
		return m_TargetData.SurfacePointData[internalTargetID(PointID, false)].Position;
	}//targetSurfacePointPosition

	bool CorrespondenceStorage::targetCorrespondencePointExists(size_t VertexID) {
		if (m_TargetData.VertexData.count(VertexID) == 0) return false;

		bool PointFound = false;
		for (auto TP : m_TargetData.VertexData.at(VertexID)) {
			if (m_TemplateData.VertexData[TP].Type == CorrespondenceType::CORRESPONDENCE) {
				PointFound = true;
				break;
			}
		}
		return PointFound;
	}//targetCorrespondencePointExists

	bool CorrespondenceStorage::targetCorrespondencePointExists(size_t Face, Vector3f SurfacePoint, size_t& CorrPoint) {
		auto TargetFace = m_TargetData.FaceToSurfacePoints.find(Face);
		if (TargetFace == m_TargetData.FaceToSurfacePoints.end()) return false;

		bool PointFound = false;
		for (auto Point : TargetFace->second) {
			float Dist = (m_TargetData.SurfacePointData[Point].Position - SurfacePoint).squaredNorm();
			
			if (Dist < (float)1e-4) {
				for (auto TP : m_TargetData.SurfacePointData[Point].TemplateVertices) {
					if (m_TemplateData.VertexData[TP].Type == CorrespondenceType::CORRESPONDENCE) {
						CorrPoint = externalTargetID(Point, false);
						PointFound = true;
						break;
					}
				}
			}
		}

		return PointFound;
	}//targetCorrespondencePointExists

	bool CorrespondenceStorage::targetFeaturePointExists(size_t VertexID) {
		if (m_TargetData.VertexData.count(VertexID) == 0) return false;

		bool PointFound = false;
		for (auto TP : m_TargetData.VertexData.at(VertexID)) {
			if (m_TemplateData.VertexData[TP].Type == CorrespondenceType::FEATURE) {
				PointFound = true;
				break;
			}
		}
		return PointFound;
	}//targetFeaturePointExists

	bool CorrespondenceStorage::targetFeaturePointExists(size_t Face, Vector3f SurfacePoint, size_t& FeatPoint) {
		auto TargetFace = m_TargetData.FaceToSurfacePoints.find(Face);
		if (TargetFace == m_TargetData.FaceToSurfacePoints.end()) return false;
		
		bool PointFound = false;
		for (auto Point : TargetFace->second) {
			float Dist = (m_TargetData.SurfacePointData[Point].Position - SurfacePoint).squaredNorm();

			if (Dist < (float)1e-4) {
				for (auto TP : m_TargetData.SurfacePointData[Point].TemplateVertices) {
					if (m_TemplateData.VertexData[TP].Type == CorrespondenceType::FEATURE) {
						FeatPoint = externalTargetID(Point, false);
						PointFound = true;
						break;
					}
				}
			}
		}

		return PointFound;
	}//targetFeaturePointExists

	void CorrespondenceStorage::eraseVertexVertexPair(size_t TemplateVertexID, bool EraseQuickAccess, std::vector<size_t>* RemainingTargetPartners) {
		// remove target correspondence data
		if (m_TemplateData.VertexData[TemplateVertexID].Type == CorrespondenceType::NONE) throw CForgeExcept("No fitting template vertex found");

		size_t TargetPoint = m_TemplateData.VertexData[TemplateVertexID].TargetPoint;

		// remove reference to template correspondence point
		for (int i = 0; i < m_TargetData.VertexData.at(TargetPoint).size(); ++i) {
			if (m_TargetData.VertexData.at(TargetPoint)[i] == TemplateVertexID) {
				m_TargetData.VertexData.at(TargetPoint).erase(m_TargetData.VertexData.at(TargetPoint).begin() + i);
				break;
			}	
		}
		
		if (RemainingTargetPartners != nullptr) (*RemainingTargetPartners) = m_TargetData.VertexData.at(TargetPoint);

		if (m_TargetData.VertexData.at(TargetPoint).empty()) // remove unique target correspondence point if no longer used by any correspondence pairs
			m_TargetData.VertexData.erase(TargetPoint);

		// remove template correspondence data

		if (EraseQuickAccess) {
			auto CorrType = m_TemplateData.VertexData[TemplateVertexID].Type;
			if (CorrType == CorrespondenceType::CORRESPONDENCE) m_TemplateData.CorrQuickAccess.erase(TemplateVertexID);
			else m_TemplateData.FeatQuickAccess.erase(TemplateVertexID);
		}

		m_TemplateData.VertexData[TemplateVertexID].Type = CorrespondenceType::NONE;
	}//eraseVertexVertexPair

	void CorrespondenceStorage::eraseVertexSurfacePointPair(size_t TemplateVertexID, bool EraseQuickAccess, std::vector<size_t>* RemainingTargetPartners) {
		// remove target correspondence data
		if (m_TemplateData.VertexData[TemplateVertexID].Type == CorrespondenceType::NONE) throw CForgeExcept("No fitting template vertex found");

		size_t TargetPointID = m_TemplateData.VertexData[TemplateVertexID].TargetPoint;
		auto& TargetSurfacePoint = m_TargetData.SurfacePointData[TargetPointID];
		
		for (int i = 0; i < TargetSurfacePoint.TemplateVertices.size(); ++i) {
			if (TargetSurfacePoint.TemplateVertices[i] == TemplateVertexID) {
				TargetSurfacePoint.TemplateVertices.erase(TargetSurfacePoint.TemplateVertices.begin() + i); // remove reference to template correspondence point
				break;
			}
		}
		
		if (RemainingTargetPartners != nullptr) (*RemainingTargetPartners) = TargetSurfacePoint.TemplateVertices;

		if (TargetSurfacePoint.TemplateVertices.empty()) { // remove unique target correspondence point if no longer used by any correspondence pairs
			for (auto Face : TargetSurfacePoint.TargetFaces) {
				auto& FaceMapping = m_TargetData.FaceToSurfacePoints.at(Face);
				
				for (int i = 0; i < FaceMapping.size(); ++i)
					if (FaceMapping[i] == TargetPointID) {
						FaceMapping.erase(FaceMapping.begin() + i); // remove TargetPointID from face
						break;
					}
				
				if (FaceMapping.empty()) m_TargetData.FaceToSurfacePoints.erase(Face);
			}

			TargetSurfacePoint.Position = Vector3f::Zero();
			TargetSurfacePoint.TargetFaces.clear();
			TargetSurfacePoint.TemplateVertices.clear();

			m_TargetData.FreeSurfacePoints.insert(TargetPointID);
		}

		// remove template correspondence data

		if (EraseQuickAccess) {
			auto CorrType = m_TemplateData.VertexData[TemplateVertexID].Type;
			if (CorrType == CorrespondenceType::CORRESPONDENCE) m_TemplateData.CorrQuickAccess.erase(TemplateVertexID);
			else m_TemplateData.FeatQuickAccess.erase(TemplateVertexID);
		}

		m_TemplateData.VertexData[TemplateVertexID].Type = CorrespondenceType::NONE;
	}//eraseVertexSurfacePointPair

	std::array<bool, 3> CorrespondenceStorage::surfacePointNearEdge(Vector3f TargetPosition, Vector3f BaryCoords) {
		std::array<bool, 3> CloseEdges = { false, false, false };
		if (BaryCoords.x() < 0.005f) CloseEdges[1] = true;
		if (BaryCoords.y() < 0.005f) CloseEdges[2] = true;
		if (BaryCoords.z() < 0.005f) CloseEdges[0] = true;
		return CloseEdges;
	}//surfacePointNearEdge

	size_t CorrespondenceStorage::externalTargetID(size_t TargetPoint, bool IsVertex) const {
		return (IsVertex) ? TargetPoint : TargetPoint + m_TargetData.VertexCount;
	}//externalTargetCorrID

	size_t CorrespondenceStorage::internalTargetID(size_t TargetPoint, bool IsVertex) const {
		return (IsVertex) ? TargetPoint : TargetPoint - m_TargetData.VertexCount;
	}//internalTargetID
}