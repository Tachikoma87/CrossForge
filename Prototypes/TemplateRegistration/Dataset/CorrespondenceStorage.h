#ifndef __TEMPREG_CORRESPONDENCESTORAGE_H__
#define __TEMPREG_CORRESPONDENCESTORAGE_H__

#include <Eigen/Eigen>

#include <set>
#include <array>

using namespace Eigen;

namespace TempReg {

	enum class CorrespondenceType {
		NONE,
		FEATURE,
		CORRESPONDENCE
	};

	class CorrespondenceStorage {
	public:
		struct Correspondence {
			size_t TemplatePointUID;
			size_t TargetPointUID;
			bool VertexVertexPair;
			CorrespondenceType Type;
		};

		CorrespondenceStorage(size_t TemplateVertexCount, size_t TargetVertexCount);
		~CorrespondenceStorage();

		void addCorrespondence(CorrespondenceType CT, size_t TemplateVertexID, size_t TargetVertexID);
		void addCorrespondence(CorrespondenceType CT, size_t TemplateVertexID, class DatasetGeometryData& TargetGeometry, size_t TargetFace, Vector3f SurfacePoint, Vector3f TargetBaryCoords);
		void removeCorrespondence(size_t TemplateVertexID, std::vector<size_t>* RemainingTargetPartners = nullptr);
		void clearCorrespondences(CorrespondenceType CT = CorrespondenceType::NONE);
		// Returns true, if given template vertex is not part of an existing correspondence / feature pair
		bool templateVertexAvailable(size_t TemplateVertexID) const;
		bool correspondenceAtTemplateVertex(Correspondence& Result, size_t Vertex) const;
		bool correspondencesAtTargetVertex(std::vector<Correspondence>& Result, size_t Vertex) const;
		bool correspondencesAtTargetMeshPoint(std::vector<Correspondence>& Result, size_t TargetFace, Vector3f SelectionPoint, float Tolerance = (float)1e-6) const;
		const std::set<size_t>& correspondencesQuickAccess(void) const;
		const std::set<size_t>& featuresQuickAccess(void) const;
		Vector3f targetSurfacePointPosition(size_t PointID) const;
		bool targetCorrespondencePointExists(size_t VertexID);
		bool targetCorrespondencePointExists(size_t Face, Vector3f SurfacePoint, size_t& CorrPoint);
		bool targetFeaturePointExists(size_t VertexID);
		bool targetFeaturePointExists(size_t Face, Vector3f SurfacePoint, size_t& FeatPoint);


	private:
		struct TemplateVertexData {
			size_t TargetPoint;
			bool TargetPointIsVertex;
			CorrespondenceType Type;

			TemplateVertexData() : TargetPoint(0), TargetPointIsVertex(false), Type(CorrespondenceType::NONE) {}
		};

		struct TemplateData {
			std::vector<TemplateVertexData> VertexData;
			std::set<size_t> FeatQuickAccess;
			std::set<size_t> CorrQuickAccess;
		};

		struct TargetSurfacePointData {
			Vector3f Position;
			std::vector<size_t> TemplateVertices;
			std::vector<size_t> TargetFaces;

			TargetSurfacePointData() : Position(Vector3f::Zero()) {}
		};

		struct TargetData {
			size_t VertexCount;
			std::map<size_t, std::vector<size_t>> VertexData;
			std::vector<TargetSurfacePointData> SurfacePointData;
			std::set<size_t> FreeSurfacePoints;
			std::map<size_t, std::vector<size_t>> FaceToSurfacePoints;
		};

		void eraseVertexVertexPair(size_t TemplateVertexID, bool EraseQuickAccess, std::vector<size_t>* RemainingTargetPartners);
		void eraseVertexSurfacePointPair(size_t TemplateVertexID, bool EraseQuickAccess, std::vector<size_t>* RemainingTargetPartners = nullptr);

		std::array<bool, 3> surfacePointNearEdge(Vector3f TargetPosition, Vector3f BaryCoords);
		size_t externalTargetID(size_t TargetPoint, bool IsVertex) const;
		size_t internalTargetID(size_t TargetPoint, bool IsVertex) const;

		TemplateData m_TemplateData;
		TargetData m_TargetData;
	};
}

#endif