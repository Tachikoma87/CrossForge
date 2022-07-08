#ifndef __TEMPREG_DATASETGEOMETRYDATA_H__
#define __TEMPREG_DATASETGEOMETRYDATA_H__

#include <igl/embree/EmbreeIntersector.h>
//#include <igl/AABB.h>
//#include <igl/octree.h>

#include "DatasetCommon.h"

#include <Eigen/Eigen>

#include <set>

using namespace Eigen;

namespace TempReg {

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
		struct VertAdjacencyRefRange {
			size_t FirstRef;
			size_t LastRef;

			VertAdjacencyRefRange() :
				FirstRef(0), LastRef(0) {}
		};

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
}

#endif