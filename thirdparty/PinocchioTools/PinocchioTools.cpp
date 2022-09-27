#include "PinocchioTools.hpp"

#include "windows.h"
#include "PinocchioTools.hpp"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

namespace nsPinocchioTools {

	void pnSkeleton::PinitCompressed() {
		initCompressed();
	}
	map<string, int> pnSkeleton::getJointNames() {
		return jointNames;
	}
	void pnSkeleton::setJointNames(map<string, int> jointNames) {
		this->jointNames = jointNames;
	}
	void pnSkeleton::PmakeJoint(const string& name, const Vector3& pos, const string& previous) {
		makeJoint(name,pos,previous);
	}
	void pnSkeleton::PmakeSymmetric(const string& name1, const string& name2) {
		makeSymmetric(name1, name2);
	}
	void pnSkeleton::PsetFoot(const string& name) {
		setFoot(name); 
	}
	void pnSkeleton::PsetFat(const string& name) {
		setFat(name);
	}

	std::vector<CForge::T3DMesh<float>::Bone*> gatherBones(CForge::T3DMesh<float>::Bone* root) {
		std::vector<CForge::T3DMesh<float>::Bone*> ret;
		std::vector<CForge::T3DMesh<float>::Bone*> queue;
		queue.push_back(root);
		while (!queue.empty()) {
			CForge::T3DMesh<float>::Bone* cur = queue[0];
			queue.erase(queue.begin());
			ret.push_back(cur);
			for (uint32_t i = 0; i < cur->Children.size(); i++) {
				queue.push_back(cur->Children[i]);
			}
		}
		return ret;
	}
	
	void convertSkeleton(CForge::T3DMesh<float>::Bone* in, nsPinocchio::Skeleton* out, CVScalingInfo* CVSInfo,
	                     std::vector<BonePair> symmetry,
	                     std::vector<CForge::T3DMesh<float>::Bone*> fat,
	                     std::vector<CForge::T3DMesh<float>::Bone*> foot,
		std::vector<Eigen::Vector3f>* joints) {
		std::vector<CForge::T3DMesh<float>::Bone*> inList = gatherBones(in);
		
		// scale skeleton to NDC and center
		// bounding box of skeleton
		Eigen::Vector3f min,max,center,d;
		for (uint32_t i = 0; i < inList.size(); i++) {
			Eigen::Matrix4f mat = inList[i]->OffsetMatrix.inverse();
			Eigen::Matrix3f rot = mat.block<3,3>(0,0);
			Eigen::Vector3f pos = Eigen::Vector3f(-mat.data()[12],mat.data()[13],mat.data()[14]);
			std::cout << inList[i]->Name << "\n";
			std::cout << mat << "\n";
			std::cout << pos << "\n\n";
			//joints->push_back(Eigen::Vector3f(pos[0],pos[1],pos[2]));
			if (i==0)
				min = max = pos;
			for (uint32_t j = 0; j < 3; j++) {
				min[j] = std::fmin(min[j],pos[j]);
				max[j] = std::fmax(max[j],pos[j]);
			}
		}
		center = (min+max)*0.5f;
		d = max-min;
		float maxBound = 0.8f;
		CVSInfo->scaling = maxBound*2.0f/std::fmax(d[0],std::fmax(d[1],d[2]));
		CVSInfo->offset = center*CVSInfo->scaling; //TODO check for correctness
		
		// create Skeleton
		pnSkeleton* skl = (pnSkeleton*) out;
		std::cout << "\n\n\n\n";
		
		for (uint32_t i = 0; i < inList.size(); i++) {
			Eigen::Matrix4f mat = inList[i]->OffsetMatrix.inverse();
			Vector3 o = Vector3(CVSInfo->offset[0],CVSInfo->offset[1],CVSInfo->offset[2]);
			Vector3 pos = Vector3(-mat.data()[12],mat.data()[13],mat.data()[14])*CVSInfo->scaling - o;
			
			joints->push_back(Eigen::Vector3f(pos[0],pos[1],pos[2]));
			
			//std::cout << inList[i]->Name << "\n";
			//for (uint32_t j = 0; j < 3; j++) {
			//	std::cout << pos[j] << "\n";
			//}
			//std::cout << "\n";
			if (inList[i]->pParent)
				skl->PmakeJoint(inList[i]->Name, pos, inList[i]->pParent->Name);
			else
				skl->PmakeJoint(inList[i]->Name, pos);
		}
		
		for (uint32_t i = 0; i < symmetry.size(); i++)
			skl->PmakeSymmetric(symmetry[i].pair[0]->Name,symmetry[i].pair[1]->Name);
		
		skl->PinitCompressed();
		
		for (uint32_t i = 0; i < foot.size(); i++)
			skl->PsetFoot(foot[i]->Name);
		for (uint32_t i = 0; i < fat.size(); i++)
			skl->PsetFat(foot[i]->Name);
	}

	void convertSkeleton(const nsPinocchio::Skeleton& in, CForge::T3DMesh<float>::Bone* out, const CVScalingInfo& CVSInfo,
	                     const nsPinocchio::Attachment& attatchment) {
		if (!&attatchment)
			throw CForgeExcept("PinocchioTools: convertSkeleton attatchment not valid");
		pnSkeleton* skl = (pnSkeleton*) &in;
		std::vector<CForge::T3DMesh<float>::Bone*> outList = gatherBones(out);
		
		// TODO real conversion?
		std::map<std::string, int> jointNames = skl->getJointNames();
		auto itr = jointNames.begin();
		while (itr != jointNames.end()) {
			//std::cout << itr->first << "\n";
			for (uint32_t i = 0; i < outList.size(); i++) {
				if (outList[i]->Name.compare(itr->first) == 0) {
					//TODO check push back weights
					Vector<double,-1> weights = attatchment.getWeights(itr->second);
					std::vector<float> wc;
					for (uint32_t j = 0; j < weights.size(); j++) {
						wc.push_back(weights[j]);
					}
					outList[i]->VertexWeights = wc;
				}
			}
			itr++;
		}
	}
}