
#include "../crossforge/AssetIO/T3DMesh.hpp"

namespace CForge {

/**
 * \class SkeletonConverter
 * \brief Class for converting between different hierarchical Formats.
 * Currently converts Bone::OffsetMatrix (Root to Bone for linear Blend Skinning) to a Hierarical format (Translation relative to Parent).
*/
class SkeletonConverter {
public:
	//TODO move into T3DMesh.hpp
	std::vector<T3DMesh<float>::Bone*> copyBones(std::vector<T3DMesh<float>::Bone*>* pBones);
	
	/**
	 * \brief Collects all Children and Subchildren of a Bone.
	 * \param ret Empty vector, containing all children after completion.
	 * \param bone Root Bone from where Children will be read from.
	*/
	static void collectBones(std::vector<T3DMesh<float>::Bone*>* ret,T3DMesh<float>::Bone* bone);

	/**
	 * \brief Creates Bone Hierarchy with OffsetMatrices relative to parent instead of Root.
	 * \param root Root Bone of the Skeleton which gets converted.
	*/
	void OMtoRH(T3DMesh<float>::Bone* root);

	T3DMesh<float>::Bone* getRoot() { return m_root; }
	std::vector<T3DMesh<float>::Bone*> getSkeleton() { return m_skeleton; }

	SkeletonConverter() : m_root(nullptr) {}
	~SkeletonConverter();
private:
	/**
	 * \brief Helper function for OffsetMatToRealtiveHierarchy.
	 * \param bone current Bone
	 * \param accu accumulated rotation from root up to parent.
	*/
	void OMtoRHwrite(T3DMesh<float>::Bone* targetBone, T3DMesh<float>::Bone* sourceBone);

	/**
	 * \brief Helper function for OffsetMatToRealtiveHierarchy.
	 * \param bone current Bone
	 * \param accu accumulated rotation from root up to parent.
	*/
	void OMtoRHrotate(T3DMesh<float>::Bone* bone, Eigen::Matrix3f accu);

	std::vector<T3DMesh<float>::Bone*> m_skeleton;
	T3DMesh<float>::Bone* m_root;
};

}//CForge

