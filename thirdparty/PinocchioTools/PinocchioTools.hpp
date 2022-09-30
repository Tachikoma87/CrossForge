#pragma once
#include "../../CForge/AssetIO/T3DMesh.hpp"
#include "../Pinocchio/pinocchioApi.h"
#include "../Pinocchio/graphutils.h"
#include "../Pinocchio/attachment.h"

#ifdef _WIN32
#define PINOCCHIOTOOLS_API __declspec(dllexport)
#else //_WIN32
#define PINOCCHIOTOOLS_API 
#endif //_WIN32

using namespace CForge;
// tools for converting engine format to pinocchio and vice versa
namespace nsPinocchioTools {
	
	// wrapper class for converting
	class pnSkeleton : nsPinocchio::Skeleton {
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
	void PINOCCHIOTOOLS_API convertSkeleton(CForge::T3DMesh<float>::Bone* in, nsPinocchio::Skeleton* out, CVScalingInfo* CVSInfo,
	                     std::vector<BonePair> symmetry,
	                     std::vector<CForge::T3DMesh<float>::Bone*> fat,
	                     std::vector<CForge::T3DMesh<float>::Bone*> foot,
	                     Eigen::Matrix3f rotation,
		std::vector<Eigen::Vector3f>* joints);

	/*
	* @param in Pinocchio Output
	* @param out T3DMesh root Bone
	*/
	void PINOCCHIOTOOLS_API adaptSkeleton( nsPinocchio::PinocchioOutput* in, CForge::T3DMesh<float>::Bone* out);
	
	/*
	* @param out 
	* @param CVSInfo scaling of skeleton
	*/
	void PINOCCHIOTOOLS_API applyWeights(CForge::T3DMesh<float>* out, const CVScalingInfo& CVSInfo,
	                                        nsPinocchio::PinocchioOutput& piO, uint32_t vertexCount);

	void PINOCCHIOTOOLS_API convertMesh(CForge::T3DMesh<float>* in, nsPinocchio::Mesh* out);
	
	void PINOCCHIOTOOLS_API autorigCust(const nsPinocchio::Skeleton &given, const nsPinocchio::Mesh &m,
		std::vector<Eigen::Vector3f>* poss, std::vector<float>* rads) {
		nsPinocchio::Mesh newMesh = prepareMesh(m);

		if(newMesh.vertices.size() == 0)
			return;

		nsPinocchio::TreeType *distanceField = constructDistanceField(newMesh);

		//discretization
		vector<nsPinocchio::Sphere> medialSurface = sampleMedialSurface(distanceField);

		vector<nsPinocchio::Sphere> spheres = packSpheres(medialSurface);
		for (uint32_t i = 0; i < spheres.size(); i++) {
			poss->push_back(Eigen::Vector3f(spheres[i].center[0],spheres[i].center[1],spheres[i].center[2]));
			rads->push_back(spheres[i].radius);
		}
	};

	void fixDupFaces(nsPinocchio::Mesh* mesh);
}