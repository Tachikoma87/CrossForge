#ifndef __TEMPREG_OBBOCTREE_H__
#define __TEMPREG_OBBOCTREE_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"

#include <Eigen/Eigen>

using namespace Eigen;

namespace TempReg {

	struct OBB {
		Vector3f Center;
		Vector3f LocalAxes[3];
		Vector3f HalfwidthExtents;

		OBB() :
			Center(Vector3f(0.0f, 0.0f, 0.0f)), LocalAxes{ Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f) }, 
			HalfwidthExtents(Vector3f(0.0f, 0.0f, 0.0f)) {}

		// /*return type*/ rotate(Quaternionf Rotation);
		// /*return type*/ translate(Vector3f Translation);
		// /*return type*/ scale(float Scaling);
	};

	struct RayPickResult {
		//TODO
	};

	class OBBOctree {
	public:
		OBBOctree();
		~OBBOctree();

		void buildOctree(CForge::T3DMesh<float>* Mesh); //TODO
		///*return type */ rotateOctree(Quaternionf Rotation); //TODO
		///*return type*/ translateOctree(Vector3f Translation); //TODO
		//RayPickResult raypick(Vectorf3f RayOrigin, Vector3f RayDir); //TODO

	private:
		struct OctreeNode {
			int32_t FirstChild;
			int32_t ElementRefCount;

			OctreeNode() : FirstChild(-1), ElementRefCount(0) {}
		};

		struct OctreeElementRef
		{
			int32_t NextElementRef;
			size_t Element;

			OctreeElementRef() : NextElementRef(-1), Element(-1) {}
		};
				
		//OBB calcChildBV(int32_t Child, OBB& ParentBV); //TODO
		//bool triangleBVIntersect(/*triangle data*/, OBB& NodeBV); //TODO
		
		CForge::T3DMesh<float> m_BVModel;

		OBB m_RootNodeBV;
		
		std::vector<OctreeNode> m_TreeNodes;
		std::vector<OctreeElementRef> m_ElementReferences;
		std::vector<size_t> m_ExternalElementIDs;
		int8_t m_MaxLayers;

		Quaternionf m_VisRotation;
		Vector3f m_VisTranslation;
	};
}

#endif