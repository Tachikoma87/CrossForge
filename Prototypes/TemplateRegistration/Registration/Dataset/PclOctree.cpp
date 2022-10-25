#include "PclOctree.h"

#include "../../CForge/Core/CrossForgeException.h"

#include <deque>
#include <set>

namespace TempReg {

	// Useful lists of numbers
	const Matrix<int, 8, 1> PclOctree::m_ZeroToSeven = (Matrix<int, 8, 1>() << 0, 1, 2, 3, 4, 5, 6, 7).finished();
	const Matrix<int, 8, 1> PclOctree::m_NegativeOnes = Matrix<int, 8, 1>::Constant(-1);

	PclOctree::PclOctree() {
		m_MaxDepth = 0;
		m_NodeCount = 0;
		m_PointBoundingRadius = m_SqPointBoundingRadius = -1.0f;
	}//Constructor

	PclOctree::~PclOctree() {

	}//Destructor

	void PclOctree::init(const MatrixXf& Points, float PointBoundingRadius, uint32_t MaxDepth) {
		if (Points.rows() == 0) throw CForgeExcept("Vertices missing");
		if (PointBoundingRadius < 0.0f) throw CForgeExcept("Negative bounding radius");

		m_MaxDepth = MaxDepth;
		m_PointBoundingRadius = PointBoundingRadius;
		m_SqPointBoundingRadius = PointBoundingRadius * PointBoundingRadius;

		generate(Points);
	}//init

	void PclOctree::init(const PclOctree& Other) {
		m_MaxDepth = Other.m_MaxDepth;
		m_PointBoundingRadius = Other.m_PointBoundingRadius;
		m_SqPointBoundingRadius = Other.m_SqPointBoundingRadius;
		m_NodeCount = Other.m_NodeCount;
		m_PointIndices = Other.m_PointIndices;
		m_NodeChildren = Other.m_NodeChildren;
		m_NodeCenters = Other.m_NodeCenters;
		m_NodeWidths = Other.m_NodeWidths;
	}//init

	void PclOctree::generate(const MatrixXf& Points) {
		m_NodeCount = 0;
		m_PointIndices.clear();
		m_NodeChildren.resize(0, 0);
		m_NodeCenters.resize(0, 0);
		m_NodeWidths.resize(0);

		std::vector<Matrix<int, 8, 1>, Eigen::aligned_allocator<Matrix<int, 8, 1>>> TempNodeChildren;
		std::vector<Vector3f> TempNodeCenters;
		std::vector<float> TempNodeWidths;

		// Build octree root:
		// -> Store all points in root node (m_PointIndices[0])
		std::vector<uint32_t> AllPoints(Points.rows());
		for (int i = 0; i < AllPoints.size(); ++i) AllPoints[i] = i;

		m_PointIndices.emplace_back(AllPoints);

		AllPoints.clear();

		// -> Preprare children of root node, defaulting indices to -1
		TempNodeChildren.emplace_back(m_NegativeOnes);

		// -> Get the minimum AABB for the points
		Vector3f AABBMin = Points.colwise().minCoeff().transpose();
		AABBMin.array() -= m_PointBoundingRadius;
		Vector3f AABBMax = Points.colwise().maxCoeff().transpose();
		AABBMax.array() += m_PointBoundingRadius;
		Vector3f RootAABBCenter = (AABBMin + AABBMax) * 0.5f;
		float RootWidth = (AABBMax - AABBMin).maxCoeff();
		TempNodeCenters.emplace_back(RootAABBCenter);

		// -> Widths are the side length of the cube (not half the side length):
		TempNodeWidths.emplace_back(RootWidth);

		// -> Count root node
		++m_NodeCount;

		// Now execute octree generation:
		splitNode(0, 0, Points, TempNodeChildren, TempNodeCenters, TempNodeWidths);

		// Finalize octree by converting from vectors to Eigen matrices:
		m_NodeChildren.resize(TempNodeChildren.size(), 8);
		m_NodeCenters.resize(TempNodeCenters.size(), 3);
		m_NodeWidths.resize(TempNodeWidths.size(), 1);

		for (int i = 0; i < TempNodeChildren.size(); i++) m_NodeChildren.row(i) = TempNodeChildren[i];
		for (int i = 0; i < TempNodeCenters.size(); i++) m_NodeCenters.row(i) = TempNodeCenters[i];
		for (int i = 0; i < TempNodeWidths.size(); i++) m_NodeWidths(i) = TempNodeWidths[i];

		TempNodeChildren.clear();
		TempNodeCenters.clear();
		TempNodeWidths.clear();
	}//generate

	void PclOctree::clear(void) {
		m_MaxDepth = 0;
		m_NodeCount = 0;
		m_PointBoundingRadius = 0.0f;
		m_SqPointBoundingRadius = 0.0f;

		for (auto& Node : m_PointIndices) Node.clear();
		m_PointIndices.clear();

		m_NodeChildren.resize(0, 0);
		m_NodeCenters.resize(0, 0);
		m_NodeWidths.resize(0);
	}//clear

	const int PclOctree::raycastClosestPoint(const Vector3f& RayOrigin, const Vector3f& RayDir, const MatrixXf& Points) const {
		int Hit = -1;
		std::set<int> Hits;
		std::deque<int> NodesToTest;
		NodesToTest.push_back(0);

		// collect potential hits
		while (!NodesToTest.empty()) {
			const auto Testing = NodesToTest.front();

			if (intersectsRayNode(RayOrigin, RayDir, Testing)) { // ray hits octree node
				const auto& Children = m_NodeChildren.row(Testing);

				if (Children(0, 0) == -1) { // node is leaf, holds points
					for (const auto& P : m_PointIndices[Testing]) {
						const Vector3f Point = Points.row(P);
						if (intersectsRaySphere(RayOrigin, RayDir, Point, m_SqPointBoundingRadius)) Hits.insert(P);
					}
				}
				else { // descend the tree
					for (int i = 0; i < 8; ++i) NodesToTest.push_back(Children(0, i));
				}
			}

			NodesToTest.pop_front();
		}

		// select closest point to ray
		float MinSqDist = FLT_MAX;
		for (const auto& P : Hits) {
			const Vector3f Point = Points.row(P);
			float SqDistToP = minSqDistPointToRay(RayOrigin, RayDir, Point);

			if (SqDistToP < MinSqDist) {
				Hit = P;
				MinSqDist = SqDistToP;
			}
		}

		return Hit;
	}//raycastClosestPoint

	const std::vector<std::vector<uint32_t>>& PclOctree::pointIndices(void) const {
		return m_PointIndices;
	}//octreePointIndices

	const MatrixXi& PclOctree::nodeChildren(void) const {
		return m_NodeChildren;
	}//octreeNodeChildren

	const MatrixXf& PclOctree::nodeCenters(void) const {
		return m_NodeCenters;
	}//octreeNodeCenters

	const VectorXf& PclOctree::nodeWidths(void) const {
		return m_NodeWidths;
	}//octreeNodeWidths

	const uint32_t PclOctree::maxDepth(void) const {
		return m_MaxDepth;
	}//maxDepth

	const float PclOctree::pointBoundingRadius(void) const {
		return m_PointBoundingRadius;
	}//octreePointBoundingRadius

	void PclOctree::splitNode(const int32_t NodeIndex, const uint32_t Depth, const MatrixXf& Points, std::vector<Matrix<int, 8, 1>, Eigen::aligned_allocator<Matrix<int, 8, 1>>>& TempNodeChildren, std::vector<Vector3f>& TempNodeCenters, std::vector<float>& TempNodeWidths) {
		if (m_PointIndices[NodeIndex].size() > 8 && Depth < m_MaxDepth) {

			// Give the parent access to the children
			TempNodeChildren[NodeIndex] = m_ZeroToSeven.array() + m_NodeCount;

			// Add the children to the lists as default children
			float ChildrenWidth = TempNodeWidths[NodeIndex] / 2;
			Vector3f ParentCenter = TempNodeCenters[NodeIndex];

			for (int i = 0; i < 8; ++i) {
				TempNodeChildren.emplace_back(m_NegativeOnes);
				m_PointIndices.emplace_back(std::vector<uint32_t>());
				TempNodeCenters.emplace_back(translateChildCenter(ParentCenter, ChildrenWidth, i));
				TempNodeWidths.emplace_back(ChildrenWidth);
			}

			// Distribute points to child nodes
			for (int i = 0; i < m_PointIndices[NodeIndex].size(); ++i) {
				int ContainedPoint = m_PointIndices[NodeIndex][i];
				auto ContainingNodes = getOctants(Points.row(ContainedPoint), NodeIndex, TempNodeChildren, TempNodeCenters, TempNodeWidths);

				for (auto Node : ContainingNodes) m_PointIndices[Node].emplace_back(ContainedPoint);
			}

			// !!! Not included in libigl's octree implementation:
			// Remove points from split node
			m_PointIndices[NodeIndex].clear();

			// Increase node count
			m_NodeCount += 8;

			// Recursion
			for (int i = 0; i < 8; ++i) 
				splitNode(TempNodeChildren[NodeIndex](i), Depth + 1, Points, TempNodeChildren, TempNodeCenters, TempNodeWidths);
		}
	}//splitNode

	Vector3f PclOctree::translateChildCenter(const Vector3f& ParentCenter, float& ChildrenWidth, const int& ChildIndex) {
		float HalfWidth = ChildrenWidth / 2.0;
		Vector3f CenterTranslation = Vector3f(-HalfWidth, -HalfWidth, -HalfWidth);

		// Positive x chilren are 1, 3, 5, 7 (as taken from libigl)
		if (ChildIndex % 2) CenterTranslation(0) = HalfWidth;

		// Positive y children are 2, 3, 6, 7 (as taken from libigl)
		if (ChildIndex == 2 || ChildIndex == 3 || ChildIndex == 6 || ChildIndex == 7) CenterTranslation(1) = HalfWidth;

		// Positive z children are 4, 5, 6, 7 (as taken from libigl)
		if (ChildIndex > 3) CenterTranslation(2) = HalfWidth;

		return ParentCenter + CenterTranslation;
	}//translateChildCenter

	std::vector<int> PclOctree::getOctants(const Vector3f& Point, const int ParentNodeIndex, std::vector<Matrix<int, 8, 1>, Eigen::aligned_allocator<Matrix<int, 8, 1>>>& NodeChildren, const std::vector<Vector3f>& NodeCenters, std::vector<float>& NodeWidths) {
		std::vector<int> OctantIndices;
		const auto& Children = NodeChildren[ParentNodeIndex];

		for (int i = 0; i < 8; ++i) {
			const auto& ChildCenter = NodeCenters[Children(i)];
			const auto& ChildWidth = NodeWidths[Children(i)];
			float ChildHalfWidth = ChildWidth * 0.5f;

			Vector3f U[3] = { Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f) };

			Vector3f D = Point - ChildCenter;
			Vector3f PointOnAABB = ChildCenter;

			for (int i = 0; i < 3; i++) {
				float Dist = D.dot(U[i]);
				if (Dist > ChildHalfWidth) Dist = ChildHalfWidth;
				if (Dist < -ChildHalfWidth) Dist = -ChildHalfWidth;
				PointOnAABB += Dist * U[i];
			}

			Vector3f V = PointOnAABB - Point;
			if (V.dot(V) <= m_SqPointBoundingRadius) OctantIndices.emplace_back(Children(i));
		}

		return OctantIndices;
	}//getOctants

	float PclOctree::sqDistToNode(const Vector3f& Point, const Vector3f& NodeCenter, const float& NodeWidth) {
		float SqDistToAABB = 0.0f;
		float HalfWidth = NodeWidth / 2.0f;
		Vector3f AABBMax = NodeCenter.array() + HalfWidth;
		Vector3f AABBMin = NodeCenter.array() - HalfWidth;

		for (int i = 0; i < 3; ++i) {
			if (Point(i) < AABBMin(i)) SqDistToAABB += (AABBMin(i) - Point(i)) * (AABBMin(i) - Point(i));
			if (Point(i) > AABBMax(i)) SqDistToAABB += (Point(i) - AABBMax(i)) * (Point(i) - AABBMax(i));
		}

		return SqDistToAABB;
	}//sqDistToNode

	const bool PclOctree::intersectsRayNode(const Vector3f& RayOrigin, const Vector3f& RayDir, const int NodeIndex) const {		
		const auto NodeChildren = m_NodeChildren.row(NodeIndex);

		// Get the min-max form of the node AABB
		Vector3f AABBCenter = m_NodeCenters.row(NodeIndex);
		float NodeHalfWidth = m_NodeWidths(NodeIndex) * 0.5f; // nodes are cubes
		Vector3f AABBMin(AABBCenter.x() - NodeHalfWidth, AABBCenter.y() - NodeHalfWidth, AABBCenter.z() - NodeHalfWidth);
		Vector3f AABBMax(AABBCenter.x() + NodeHalfWidth, AABBCenter.y() + NodeHalfWidth, AABBCenter.z() + NodeHalfWidth);
				
		float TFirst = 0.0f;
		float TLast = FLT_MAX;

		if (!intersectsRaySlab(RayOrigin.x(), RayDir.x(), AABBMin.x(), AABBMax.x(), TFirst, TLast)) return false;
		if (!intersectsRaySlab(RayOrigin.y(), RayDir.y(), AABBMin.y(), AABBMax.y(), TFirst, TLast)) return false;
		if (!intersectsRaySlab(RayOrigin.z(), RayDir.z(), AABBMin.z(), AABBMax.z(), TFirst, TLast)) return false;

		return true;
	}//intersectsRayNode

	const bool PclOctree::intersectsRaySlab(float RayOrigin, float RayDir, float Min, float Max, float& TFirst, float& TLast) const {
		if (std::abs(RayDir) < 1e-8) return RayOrigin < Max && RayOrigin > Min;

		double TMin = (Min - RayOrigin) / RayDir;
		double TMax = (Max - RayOrigin) / RayDir;

		if (TMin > TMax) std::swap(TMin, TMax);
		if (TMax < TFirst || TMin > TLast) return false;
		if (TMin > TFirst) TFirst = TMin;
		if (TMax < TLast) TLast = TMax;

		return true;
	}//intersectsRaySlab

	const bool PclOctree::intersectsRaySphere(const Vector3f& RayOrigin, const Vector3f& RayDir, const Vector3f& SphereCenter, const float SphereRadiusSq) const {
		Vector3f Diff = RayOrigin - SphereCenter;
		float C = Diff.dot(Diff) - SphereRadiusSq;		
		if (C <= 0.0f) return true;

		float B = Diff.dot(RayDir);
		if (B > 0.0f) return false;

		float Disc = B * B - C;
		if (Disc < 0.0f) return false;

		return true;
	}//intersectsRaySphere

	const float PclOctree::minSqDistPointToRay(const Vector3f& RayOrigin, const Vector3f& RayDir, const Vector3f& Point) const {
		float T0 = (RayDir.dot(Point - RayOrigin)) / (RayDir.dot(RayDir));
		return (T0 <= 0.0f) ? (Point - RayOrigin).squaredNorm() : (Point - (RayOrigin + T0 * RayDir)).squaredNorm();
	}//minSqDistPointToRay
}