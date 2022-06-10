
#include <igl/decimate.h>
//#include <igl/read_triangle_mesh.h>
//#include <igl/write_triangle_mesh.h>
#include "../CForge/AssetIO/T3DMesh.hpp"

namespace CForge {
	class MeshDecimator {
	
	public:
		static void decimateMesh(const CForge::T3DMesh<float>* inMesh, CForge::T3DMesh<float>* outMesh, float amount);
		
		static const uint32_t m_MaxOctreeDepth = 128;
		static const uint32_t m_MaxLeafVertexCount = 5;

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
		static void createOctree(octreeNode* pNode, Eigen::MatrixXd* DV, std::vector<std::vector<octreeNode*>>* depthNodes);
		static bool decimateOctree(Eigen::MatrixXd& DV, Eigen::MatrixXi& DF, Eigen::VectorXi* DuF, Eigen::VectorXi* DuV, uint32_t faceAmount);
		static std::vector<uint32_t> joinPoints(Eigen::MatrixXd* DV, Eigen::MatrixXi* DF, const std::vector<uint32_t>& targets);

		MeshDecimator(void);
		~MeshDecimator(void);
	private:
	};
}
