/*****************************************************************************\
*                                                                           *
* File(s): SkeletalSkinning.h and SkeletalSkinning.cpp                                            *
*                                                                           *
* Content:    *
*          .              *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/

#ifndef __CFORGE_SKELETALSKINNING_H__
#define __CFORGE_SKELETALSKINNING_H__

#include <crossforge/AssetIO/T3DMesh.hpp>
#include <crossforge/Graphics/Controller/SkeletalAnimationController.h>

namespace CForge {
	class SkeletalSkinning {
	public:
		SkeletalSkinning(void);
		~SkeletalSkinning(void);

		void init(T3DMesh<float> *pMesh, SkeletalAnimationController *pController);

		Eigen::Vector3f transformVertex(int32_t Index);

	protected:
		struct SkinVertex {
			Eigen::Vector3f V;
			std::vector<int32_t> BoneInfluences;
			std::vector<float> BoneWeights;
		};//SkinVertex

		SkeletalAnimationController* m_pController;
		std::vector<SkinVertex*> m_SkinVertexes;

	};//SkeletalSkinning
}


#endif 


