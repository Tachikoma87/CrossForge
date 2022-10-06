#ifndef __TEMPREG_DATASETGEOMETRYDATA_H__
#define __TEMPREG_DATASETGEOMETRYDATA_H__

#include "../../../CForge/AssetIO/T3DMesh.hpp"

#include <igl/embree/EmbreeIntersector.h>
//#include <igl/AABB.h>
//#include <igl/octree.h>

#include "PclOctree.h"

#include <Eigen/Eigen>

using namespace Eigen;

namespace TempReg {

	class DatasetGeometryData {
	public:
		DatasetGeometryData(void);
		~DatasetGeometryData();

		void init(std::string Filepath, bool IsMesh, uint32_t MaxBVHDepth = 6, float PointBoundingRadius = 1.0);
		void init(const DatasetGeometryData& Other);
		void clear(void);
		
		void applyTransformation(const Matrix3f& M);
		void applyTransformation(const Vector3f& T);
		void applyTransformation(const Matrix3f& R, const Vector3f& T);
		void applyTransformation(const Matrix3f& S, const Matrix3f& R, const Vector3f& T);
		void computeNormals(void);
		void computeVertexCentroid(void);
		void computeSurfaceCentroid(void);
		void updateActiveBVHs(void);
		
		const Vector3f vertexPosition(uint32_t Idx) const;
		const MatrixXf& vertexPositions(void) const;
		void vertexPositions(const MatrixXf& Vertices);
		const int32_t closestVertex(const uint32_t Face, const Vector3f SurfacePoint, float Range) const;
		size_t vertexCount(void) const;
		const Vector3i face(uint32_t Idx) const;
		const MatrixXi& faces(void) const;
		size_t faceCount(void) const;
		const Vector3f vertexNormal(uint32_t Idx) const;
		const Vector3f faceNormal(uint32_t Idx) const;
		const Vector3f vertexCentroid(void) const;
		const Vector3f surfaceCentroid(void) const;
		
		void raycastMeshPoint(const Vector2f& CursorPosOGL, const Vector4f& Viewport, const Matrix4f& Model, const Matrix4f& View, const Matrix4f& Projection, int32_t& IntersectedFace, Vector3f& IntersectionPoint, Vector3f& BaryCoords) const;
		void raycastPclPoint(const Vector2f& CursorPosOGL, const Vector4f& Viewport, const Matrix4f& Model, const Matrix4f& View, const Matrix4f& Projection, int32_t& PickedPoint) const;

	private:
		void computeFaceNormals(void);
		void computeVertexNormals(void);
		
		bool m_IsMesh;
		Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> m_Positions;
		Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> m_VertexNormals;
		Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> m_FaceNormals;
		Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> m_Faces;
		Vector3f m_VertexCentroid;
		Vector3f m_SurfaceCentroid;

		//TODO: if half-edge data structure like iterations / adjacency-based processing are necessary -> libigl offers several functions (search for "HalfEdgeIterator", "adjacency")
		
		// EmbreeIntersector contains BVH used for raycasts against meshes
		igl::embree::EmbreeIntersector m_EI;

		// TODO MeshClpBVH -> EmbreeIntersector does not support closest point computations, igl::AABB-Tree required
		//igl::AABB<Eigen::MatrixXf, 3> m_MeshClpBVH;

		// PclOctree -> EmbreeIntersector and igl::AABB do not support point clouds
		PclOctree m_PclBVH;
	};
}

#endif