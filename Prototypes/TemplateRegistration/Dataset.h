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

using namespace Eigen;

namespace TempReg {
	
	enum class DatasetType : int8_t {
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

	enum class DatasetGeometryType : int8_t {
		MESH,
		POINTCLOUD
	};

	class DatasetGeometryData {
	public:
		DatasetGeometryData(void);
		~DatasetGeometryData();

		void initFromFile(std::string Filepath, DatasetGeometryType GT);
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
		const igl::embree::EmbreeIntersector& embreeIntersector(void) const;

	private:
		DatasetGeometryType m_GeometryType;

		MatrixXd m_Vertices;
		MatrixXd m_VertexNormals;
		MatrixXi m_Faces;
		// TODO Half-Edge datastructure for meshes
		// TODO vertex normals, face normals?
		
		// EmbreeIntersector contains BVH used for raycasts against meshes
		igl::embree::EmbreeIntersector m_EI;
		
		// TODO MeshClpBVH -> EmbreeIntersector does not support closest point computations, igl::AABB-Tree required
		//igl::AABB<Eigen::MatrixXd, 3> m_MeshClpBVH;

		// TODO PclBVH -> EmbreeIntersector and igl::AABB do not support point clouds, igl::octree required for all use cases (picking, normal shooting, closest point computation)
		//std::vector<std::vector<int32_t>> m_PclOctreePointIndices;	// from libigl -> octree.h: a vector of vectors, where the ith entry is a vector of the indices into P that are the ith octree cell's points
		//MatrixXi m_PclOctreeCellChildren;							// from libigl -> octree.h: dimensions [#OctreeCells by 8]; the ith row is the indices of the ith octree cell's children
		//MatrixXd m_PclOctreeCellCenter;							// from libigl -> octree.h: dimensions [#OctreeCells by 3]; the ith row is a 3d row vector representing the position of the ith cell's center
		//VectorXd m_PclOctreeCellWidth;							// from libigl -> octree.h: dimensions [#OctreeCells by 1]; a vector where the ith entry is the width of the ith octree cell
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Render Data
	// 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class DatasetRenderMode : int8_t {
		FILL,
		LINE, // Wireframe Mode
		POINT,
	};

	enum class DatasetShadingMode : int8_t {
		SOLID_COLOR,
		HAUSDORFF_DISTANCE,
	};

	class DatasetActor : public CForge::IRenderableActor {
	public:
		
		DatasetActor(void);
		~DatasetActor(void);

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
		DatasetActor* pDatasetActor;
		CForge::SGNTransformation DatasetTransSGN;
		CForge::SGNGeometry DatasetGeomSGN;
		std::vector<CForge::SGNGeometry> MarkerGeomSGNs; // individual vertex marker instances; insert as children of DatasetTransSGN
	};//DatasetDisplayData
}

#endif