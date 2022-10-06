#ifndef __TEMPREG_PCLOCTREE_H__
#define __TEMPREG_PCLOCTREE_H__

#include <Eigen/Eigen>

#include <vector>

using namespace Eigen;

namespace TempReg {

	// the following methods have been adapted from libigl -> octree.cpp to allow for points with bounding spheres:
	//	- generate(...)				-> adapted from igl::octree(...)
	//	- splitNode(...)			-> adapted from 'helper' lambda expression
	//	- translateChildCenter(...) -> adapted from 'translate_center' lambda expression
	//	- getOctants(...)			-> adapted from 'get_octant' lambda expression

	class PclOctree {
	public:
		PclOctree();
		~PclOctree();

		void init(const MatrixXf& Points, float PointBoundingRadius, uint32_t MaxDepth);
		void init(const PclOctree& Other);
		void generate(const MatrixXf& Points);
		void clear(void);

		const int32_t raycastClosestPoint(const Vector3f& RayOrigin, const Vector3f& RayDir, const MatrixXf& Points) const;

		const std::vector<std::vector<uint32_t>>& pointIndices(void) const;
		const MatrixXi& nodeChildren(void) const;
		const MatrixXf& nodeCenters(void) const;
		const VectorXf& nodeWidths(void) const;
		const uint32_t maxDepth(void) const;
		const float pointBoundingRadius(void) const;

	private:
		void splitNode(const int32_t NodeIndex, const uint32_t Depth, const MatrixXf& Points, std::vector<Matrix<int, 8, 1>, Eigen::aligned_allocator<Matrix<int, 8, 1>>>& TempNodeChildren, std::vector<Vector3f>& TempNodeCenters, std::vector<float>& TempNodeWidths);
		Vector3f translateChildCenter(const Vector3f& ParentCenter, float& ChildrenWidth, const int& ChildIndex);

		// ChildrenWidth is side length of all children nodes (as per libigl implementation octree nodes are cubes)
		std::vector<int> getOctants(const Vector3f& Point, const int ParentNodeIndex, std::vector<Matrix<int, 8, 1>, Eigen::aligned_allocator<Matrix<int, 8, 1>>>& NodeChildren, const std::vector<Vector3f>& NodeCenters, std::vector<float>& NodeWidths);

		// NodeWidth is side length of octree node along all 3 sides (as per libigl implementation octree nodes are cubes)
		float sqDistToNode(const Vector3f& Point, const Vector3f& NodeCenter, const float& NodeWidth);

		const bool intersectsRayNode(const Vector3f& RayOrigin, const Vector3f& RayDir, const int NodeIndex) const;
		const bool intersectRaySlab(const float RayOrigin, const float RayDir, const float Min, const float Max, float& TFirst, float& TLast) const;
		const bool intersectRaySphere(const Vector3f& RayOrigin, const Vector3f& RayDir, const Vector3f& SphereCenter, const float SphereRadiusSq) const;

		// Useful lists of numbers
		static const Matrix<int, 8, 1> m_ZeroToSeven;
		static const Matrix<int, 8, 1> m_NegativeOnes;

		uint32_t m_MaxDepth;
		uint32_t m_NodeCount;
		float m_PointBoundingRadius;
		float m_SqPointBoundingRadius;
		std::vector<std::vector<uint32_t>> m_PointIndices;	// the ith entry in m_PointIndices contains a list of the IDs of points contained in the ith octree node (points are provided to init(...) / generate(...))
		MatrixXi m_NodeChildren;							// dimensions [#OctreeNodes by 8]; the ith row is the indices of the ith octree node's children
		MatrixXf m_NodeCenters;								// dimensions [#OctreeNodes by 3]; the ith row is a 3d row vector representing the position of the ith node's center
		VectorXf m_NodeWidths;								// dimensions [#OctreeNodes by 1]; the ith entry is the width of the ith octree node (nodes are cubes)
	};
}

#endif