#ifndef __TEMPREG_DATASET_H__
#define __TEMPREG_DATASET_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"
#include "../../CForge/Graphics/Actors/StaticActor.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include <Eigen/Eigen>
#include <igl/embree/EmbreeIntersector.h>
//#include <igl/AABB.h>
//#include <igl/octree.h>

//#include <queue>
#include <set>

using namespace Eigen;

namespace TempReg {
	
	enum class DatasetType {
		NONE,
		TEMPLATE,		// original template
		DTEMPLATE,		// deformed template
		TARGET			// target
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Geometry Data
	// 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class DatasetGeometryType {
		MESH,
		POINTCLOUD
	};

	struct VertAdjacencyRefRange {
		size_t FirstRef;
		size_t LastRef;

		VertAdjacencyRefRange() :
			FirstRef(0), LastRef(0) {}
	};

	class DatasetGeometryData {
	public:

		DatasetGeometryData(void);
		~DatasetGeometryData();

		void initFromFile(std::string Filepath, DatasetGeometryType GT);
		void initMeshAdjacencyData(void);
		void initEmbreeIntersector(void);
		//void initMeshClpBVH(void); //TODO
		//void initPclBVH(void); //TODO

		void updateActiveBVHs(void); //TODO

		DatasetGeometryType geometryType(void) const;
		const Vector3f vertex(Index Idx) const;
		const MatrixXd& vertices(void) const;
		const MatrixXd faceVertices(Index FaceIdx) const;
		size_t vertexCount(void) const;
		const Vector3i face(Index Idx) const;
		const MatrixXi& faces(void) const;
		size_t faceCount(void) const;
		int64_t faceAdjacentToEdge(size_t Face, size_t EdgeBegin) const;
		void facesAdjacentToFace(size_t OriginFace, std::vector<size_t>& AdjacentFaces, int64_t PrevOriginFace = -1, int64_t AdjacencyLayers = 1) const;
		std::vector<size_t> facesAdjacentToVertex(size_t Vertex) const;
		const igl::embree::EmbreeIntersector& embreeIntersector(void) const;

	private:
		DatasetGeometryType m_GeometryType;

		MatrixXd m_Vertices;
		MatrixXd m_VertexNormals;
		MatrixXi m_Faces;
		
		std::vector<VertAdjacencyRefRange> m_VertexFaceAdjRefMap;
		std::vector<size_t> m_VertexFaceAdjacency;
		std::vector<int64_t> m_FaceFaceAdjacency;

		// TODO Half-Edge datastructure for meshes
		// TODO vertex normals, face normals?
		
		// EmbreeIntersector contains BVH used for raycasts against meshes
		igl::embree::EmbreeIntersector m_EI;
		
		// TODO MeshClpBVH -> EmbreeIntersector does not support closest point computations, igl::AABB-Tree required
		//igl::AABB<Eigen::MatrixXd, 3> m_MeshClpBVH;

		// TODO PclBVH -> EmbreeIntersector and igl::AABB do not support point clouds, igl::octree required for all use cases (picking, normal shooting, closest point computation)
		//std::vector<std::vector<int32_t>> m_PclOctreePointIndices;	// from libigl -> octree.h: a vector of vectors, where the ith entry is a vector of the indices into P that are the ith octree cell's points
		//MatrixXi m_PclOctreeCellChildren;								// from libigl -> octree.h: dimensions [#OctreeCells by 8]; the ith row is the indices of the ith octree cell's children
		//MatrixXd m_PclOctreeCellCenter;								// from libigl -> octree.h: dimensions [#OctreeCells by 3]; the ith row is a 3d row vector representing the position of the ith cell's center
		//VectorXd m_PclOctreeCellWidth;								// from libigl -> octree.h: dimensions [#OctreeCells by 1]; a vector where the ith entry is the width of the ith octree cell
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Correspondence Data
	// 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class CorrespondenceType {
		NONE,
		FEATURE,
		CORRESPONDENCE
	};

	struct TargetCorrData {
		size_t UID;
		bool IsVertex;
		Vector3f Position;
	};

	struct CorrespondencePair {
		size_t TemplatePointUID;
		TargetCorrData TargetPoint;
		CorrespondenceType Type;
	};

	class CorrespondenceStorage {
	public:
		CorrespondenceStorage(size_t TemplateVertexCount, size_t TargetVertexCount);
		~CorrespondenceStorage();

		void addCorrespondence(CorrespondenceType CT, size_t TemplateVertexID, size_t TargetVertexID);
		void addCorrespondence(CorrespondenceType CT, size_t TemplateVertexID, DatasetGeometryData& TargetGeometry, size_t TargetFace, Vector3f SurfacePoint, Vector3f TargetBaryCoords);
		void removeCorrespondence(size_t TemplateVertexID);
		void clearCorrespondences(CorrespondenceType CT = CorrespondenceType::NONE);
		// Returns true, if given template vertex is not part of an existing correspondence / feature pair
		bool templateVertexAvailable(size_t TemplateVertexID) const;
		bool correspondenceAtTemplateVertex(CorrespondencePair& Result, size_t Vertex) const;
		bool correspondencesAtTargetVertex(std::vector<CorrespondencePair>& Result, size_t Vertex) const;
		bool correspondencesAtTargetMeshPoint(std::vector<CorrespondencePair>& Result, size_t TargetFace, Vector3f SelectionPoint, float Tolerance = (float)1e-6) const;
		const std::set<size_t>& correspondencesQuickAccess(void) const;
		const std::set<size_t>& featuresQuickAccess(void) const;

	private:
		struct TemplateVertexData {
			size_t TargetPoint;
			bool TargetPointIsVertex;
			CorrespondenceType Type;

			TemplateVertexData() : TargetPoint(0), TargetPointIsVertex(false), Type(CorrespondenceType::NONE) {}
		};

		struct TemplateData {
			size_t VertexCount;
			std::map<size_t, TemplateVertexData> VertexData;
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

		void eraseVertexVertexPair(size_t TemplateVertexID, bool EraseQuickAccess);
		void eraseVertexSurfacePointPair(size_t TemplateVertexID, bool EraseQuickAccess);
		
		std::array<bool, 3> surfacePointOnEdge(Vector3f TargetPosition, Vector3f BaryCoords);
		size_t externalTargetID(size_t TemplatePoint) const;

		TemplateData m_TemplateData;
		TargetData m_TargetData;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Render Data
	// 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class DatasetRenderMode {
		FILL,
		LINE,
		POINT
	};

	enum class DatasetShadingMode {
		SOLID_COLOR,
		HAUSDORFF_DISTANCE
	};

	class DatasetActor : public CForge::IRenderableActor {
	public:
		
		DatasetActor(void);
		~DatasetActor();

		void init(const CForge::T3DMesh<float>* pMesh, DatasetRenderMode RM);

		void release(void);
		void render(CForge::RenderDevice* pRDev);

		void renderMode(DatasetRenderMode RM);
		DatasetRenderMode renderMode(void)const;

	protected:
		CForge::StaticActor m_Actor;
		DatasetRenderMode m_RenderMode;
	};//DatasetActor

	struct DatasetDisplayData {		
		DatasetShadingMode ShadingMode;
		DatasetActor* pPrimitivesActor;
		DatasetActor* pWireframeActor;
		CForge::SGNTransformation TransSGN;
		CForge::SGNGeometry PrimitivesGeomSGN;
		CForge::SGNGeometry WireframeGeomSGN;				
	};//DatasetDisplayData
}

#endif