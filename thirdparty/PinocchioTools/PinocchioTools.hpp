#pragma once
#include "../../CForge/AssetIO/T3DMesh.hpp"
#include "../Pinocchio/pinocchioApi.h"
#include "../Pinocchio/graphutils.h"
#include "../Pinocchio/attachment.h"

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
	* @param in root of Skeleton
	* @param out 
	* @param out scaling of skeleton
	*/
	void __declspec(dllexport) convertSkeleton(CForge::T3DMesh<float>::Bone* in, nsPinocchio::Skeleton* out, CVScalingInfo* CVSInfo,
	                     std::vector<BonePair> symmetry,
	                     std::vector<CForge::T3DMesh<float>::Bone*> fat,
	                     std::vector<CForge::T3DMesh<float>::Bone*> foot,
		std::vector<Eigen::Vector3f>* joints);
	/*
	* @param in Pinocchio Skeleton
	* @param out 
	* @param in scaling of skeleton
	*/
	void __declspec(dllexport) convertSkeleton(const nsPinocchio::Skeleton& in, CForge::T3DMesh<float>::Bone* out, const CVScalingInfo& CVSInfo,
	                     const nsPinocchio::Attachment& attatchment);
}