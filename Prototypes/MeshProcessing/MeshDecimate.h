#pragma once

#include <crossforge/AssetIO/T3DMesh.hpp>

namespace CForge {
	class MeshDecimator {
	
	public:
		static bool decimateMesh(const CForge::T3DMesh<float>* inMesh, CForge::T3DMesh<float>* outMesh, float amount);
		
		static const uint32_t m_MaxOctreeDepth = 16;
		static const uint32_t m_MaxLeafVertexCount = 25;

		struct octreeNode {
			std::vector<uint32_t> VertexIDs;
			octreeNode* parent;
			octreeNode* childs[8];
			uint32_t depth;
			T3DMesh<float>::AABB BoundingBox;

			octreeNode(void) {
				VertexIDs.clear();
				parent = nullptr;
				depth = -1;
				for (uint8_t i = 0; i < 8; ++i) childs[i] = nullptr;
			}
		};
		
		static bool insideAABB(T3DMesh<float>::AABB BoundingBox, Eigen::Vector3f Vertex);
		static void releaseOctree(octreeNode* root);
		static void createOctree(octreeNode* pNode, Eigen::MatrixXd* DV, std::vector<std::unordered_map<octreeNode*,bool>>* depthNodes);
		static bool decimateOctree(Eigen::MatrixXd& DV, Eigen::MatrixXi& DF, Eigen::VectorXi* DuF, Eigen::VectorXi* DuV,
		                           uint32_t faceAmount, std::vector<std::vector<uint32_t>*> DVnoMulUsedInTri);
		static void joinPoints(Eigen::MatrixXd* DV, Eigen::MatrixXi* DF, const std::vector<uint32_t>& targets,
		                       const std::vector<std::vector<uint32_t>*>& DVnoMulUsedInTri, std::vector<uint32_t>* removedFaces);

		MeshDecimator(void);
		~MeshDecimator(void);
	private:
	};
}
