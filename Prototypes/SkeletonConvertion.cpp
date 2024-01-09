
#include "SkeletonConvertion.hpp"

namespace CForge {

//TODO move into T3DMesh.hpp and replace substitude part in bones()
std::vector<T3DMesh<float>::Bone*> SkeletonConverter::copyBones(std::vector<T3DMesh<float>::Bone*>* pBones) {
	std::vector<T3DMesh<float>::Bone*> bones;
	if (nullptr != pBones) {
		// create bones
		for (size_t i = 0; i < pBones->size(); ++i)
			bones.push_back(new T3DMesh<float>::Bone());

		for (size_t i = 0; i < pBones->size(); ++i) {
			// copy data
			bones[i]->ID = pBones->at(i)->ID;
			bones[i]->Name = pBones->at(i)->Name;
			bones[i]->Position = pBones->at(i)->Position;
			bones[i]->OffsetMatrix = pBones->at(i)->OffsetMatrix;
			bones[i]->VertexInfluences = pBones->at(i)->VertexInfluences;
			bones[i]->VertexWeights = pBones->at(i)->VertexWeights;
			
		}//for[bones]
			
		for (size_t i = 0; i < pBones->size(); ++i) {
			// create links
			if (pBones->at(i)->pParent) {
				for (auto b : bones) {
					if (b->Name == pBones->at(i)->pParent->Name)
						bones[i]->pParent = b;
				}
				//bones[i]->pParent = bones[pBones->at(i)->pParent->ID];
			}
			
			for (size_t k = 0; k < pBones->at(i)->Children.size(); ++k) {
				for (auto b : bones) {
					if (b->Name == pBones->at(i)->Children[k]->Name)
					bones[i]->Children.push_back(b);
				}
				//bones[i]->Children.push_back(bones[pBones->at(i)->Children[k]->ID]);
			}//for[children]
		}//for[bones]
	}
	return bones;
}//copyBones

void SkeletonConverter::OMtoRH(T3DMesh<float>::Bone* root)
{
	// Collect all Bones in a std::vector for copy
	std::vector<T3DMesh<float>::Bone*> m_rootSkeleton;
	collectBones(&m_rootSkeleton,root);
	
	m_skeleton = copyBones(&m_rootSkeleton);
	
	// find new root
	T3DMesh<float>::Bone* retRoot;
	for (uint32_t i = 0; i < m_skeleton.size(); ++i) {
		if (m_skeleton[i]->pParent == nullptr) {
			retRoot = m_skeleton[i];
			break;
		}
	}
	
	OMtoRHwrite(retRoot,root);
	OMtoRHrotate(retRoot,Eigen::Matrix3f::Identity());
	
	m_root = retRoot;
}

SkeletonConverter::~SkeletonConverter() {
	for (uint32_t i = 0; i < m_skeleton.size(); ++i) {
		delete m_skeleton[i];
		m_skeleton[i] = nullptr;
	}
	m_root = nullptr;
}

void SkeletonConverter::collectBones(std::vector<T3DMesh<float>::Bone*>* ret,T3DMesh<float>::Bone* bone) {
	if (bone->pParent == nullptr)
		ret->push_back(bone);
	for (uint32_t i = 0; i < bone->Children.size(); ++i) {
		ret->push_back(bone->Children[i]);
		collectBones(ret,bone->Children[i]);
	}
}

void SkeletonConverter::OMtoRHwrite(T3DMesh<float>::Bone* targetBone, T3DMesh<float>::Bone* sourceBone) {
	Eigen::Matrix4f mat = sourceBone->OffsetMatrix.inverse();
	targetBone->OffsetMatrix = mat;
	
	for (uint32_t i = 0; i < sourceBone->Children.size(); ++i) {
		OMtoRHwrite(targetBone->Children[i], sourceBone->Children[i]);
	}
	
	// replace translation part with translation relative to previous bone
	Eigen::Vector3f translation = Eigen::Vector3f(mat.data()[12], mat.data()[13], mat.data()[14]);
	if (targetBone->pParent) {
		translation[0] -= targetBone->pParent->OffsetMatrix.data()[12];
		translation[1] -= targetBone->pParent->OffsetMatrix.data()[13];
		translation[2] -= targetBone->pParent->OffsetMatrix.data()[14];
	}
	
	Eigen::Matrix4f f = mat;
	f.data()[12] = translation[0];
	f.data()[13] = translation[1];
	f.data()[14] = translation[2];
	
	targetBone->OffsetMatrix = f;
}

void SkeletonConverter::OMtoRHrotate(T3DMesh<float>::Bone* bone, Eigen::Matrix3f accu) {
	Eigen::Matrix4f mat = bone->OffsetMatrix;
	Eigen::Matrix3f rot = mat.block<3,3>(0,0);
	Eigen::Vector3f trans = Eigen::Vector3f(mat.data()[12],mat.data()[13],mat.data()[14]);
	
	trans = accu.inverse()*trans;
	rot = accu.inverse()*rot; // global to relative rotation
	accu = accu*rot;
	
	mat.block<3,3>(0,0) = rot;
	mat.data()[12] = trans[0];
	mat.data()[13] = trans[1];
	mat.data()[14] = trans[2];
	bone->OffsetMatrix = mat;
	
	for (uint32_t i = 0; i < bone->Children.size(); i++) {
		OMtoRHrotate(bone->Children[i], accu);
	}
}

}//CForge

