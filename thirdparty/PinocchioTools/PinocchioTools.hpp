
#ifdef _WIN32
#define PINOCCHIOTOOLS_API __declspec(dllexport)
#else //_WIN32
#define PINOCCHIOTOOLS_API 
#endif //_WIN32

#pragma once
#include "../../CForge/AssetIO/T3DMesh.hpp"
#include "../Pinocchio/pinocchioApi.h"
#include "../Pinocchio/graphutils.h"
#include "../Pinocchio/attachment.h"
#include "../../CForge/Graphics/GraphicsUtility.h"

using namespace CForge;
// tools for converting engine format to pinocchio and vice versa
namespace nsPinocchioTools {
	
	namespace nsPiR = nsPinocchio;
	
	// wrapper class for converting
	class pnSkeleton : nsPiR::Skeleton {
	public:
		void PinitCompressed();
		// public wrappers
		map<string, int> getJointNames();
		void setJointNames(map<string, int> jointNames);
		void PmakeJoint(const string &name, const Vector3 &pos, const string &previous = string());
		void PmakeSymmetric(const string &name1, const string &name2);
		void PsetFoot(const string &name);
		void PsetFat(const string &name);
	};
	
	struct CVScalingInfo {
		float scaling = 1.0f;
		Eigen::Vector3f offset = Eigen::Vector3f(0.0f,0.0f,0.0f);
	};
	
	struct BonePair {
		CForge::T3DMesh<float>::Bone* pair[2];
	};
	
	std::vector<CForge::T3DMesh<float>::Bone*> gatherBones(CForge::T3DMesh<float>::Bone* root);
	
	/*
	* @param in T3DMesh root Bone
	* @param out Pinocchio Skeleton
	* @param CVSInfo scaling of skeleton
	*/
	void PINOCCHIOTOOLS_API convertSkeleton(CForge::T3DMesh<float>::Bone* in, nsPiR::Skeleton* out, CVScalingInfo* CVSInfo,
	                     std::vector<BonePair> symmetry,
	                     std::vector<CForge::T3DMesh<float>::Bone*> fat,
	                     std::vector<CForge::T3DMesh<float>::Bone*> foot,
	                     Eigen::Matrix3f rotation,
		std::vector<Eigen::Vector3f>* joints);

	/*
	* @param in Pinocchio Output
	* @param out T3DMesh root Bone
	*/
	void PINOCCHIOTOOLS_API adaptSkeleton( nsPiR::PinocchioOutput* in, nsPiR::Skeleton* inSkl, CForge::T3DMesh<float>::Bone* out);
	
	/*
	* @param out 
	* @param CVSInfo scaling of skeleton
	*/
	void PINOCCHIOTOOLS_API applyWeights(nsPiR::Skeleton* in, CForge::T3DMesh<float>* out, const CVScalingInfo& CVSInfo,
		nsPiR::PinocchioOutput& piO, uint32_t vertexCount);
	
	/* morphs and scales mesh to targetSkl
	* @param in T3DMesh
	* @param pinSkl Pinocchio Skeleton
	*/
	void PINOCCHIOTOOLS_API MeshToTPose(T3DMesh<float>* in, nsPiR::Skeleton pinSkl);
	
	void PINOCCHIOTOOLS_API autorigCust(const nsPiR::Skeleton &given, const nsPiR::Mesh &m,
		std::vector<Eigen::Vector3f>* poss, std::vector<float>* rads) {
		nsPiR::Mesh newMesh = prepareMesh(m);

		if(newMesh.vertices.size() == 0)
			return;

		nsPiR::TreeType *distanceField = constructDistanceField(newMesh);

		//discretization
		vector<nsPiR::Sphere> medialSurface = sampleMedialSurface(distanceField);

		vector<nsPiR::Sphere> spheres = packSpheres(medialSurface);
		for (uint32_t i = 0; i < spheres.size(); i++) {
			poss->push_back(Eigen::Vector3f(spheres[i].center[0],spheres[i].center[1],spheres[i].center[2]));
			rads->push_back(spheres[i].radius);
		}
	};
	
	void PINOCCHIOTOOLS_API convertMesh(CForge::T3DMesh<float>* in, nsPinocchio::Mesh* out);
	
	
	// copies and scales animation
	void PINOCCHIOTOOLS_API copyAnimation(CForge::T3DMesh<float>* source, CForge::T3DMesh<float>* target, uint32_t animationIndex);
	
	void fixDupFaces(nsPiR::Mesh* mesh);
}