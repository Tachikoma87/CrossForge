/*****************************************************************************\
*                                                                           *
* File(s): SkeletalAnimationController.h and SkeletalAnimationController.cpp*
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SKELETALANIMATIONCONTROLLER_H__
#define __CFORGE_SKELETALANIMATIONCONTROLLER_H__

#include "../../AssetIO/T3DMesh.hpp"
#include "../UniformBufferObjects/UBOBoneData.h"
#include "../Shader/ShaderCode.h"
#include "../Shader/GLShader.h"

namespace CForge {
	class CFORGE_API SkeletalAnimationController: public CForgeObject {
	public:
		struct Animation {
			int32_t AnimationID;
			float Speed;
			float t; //current
			float TicksPerSecond;
			bool Finished;
		};

		SkeletalAnimationController(void);
		~SkeletalAnimationController(void);

		// pMesh has to hold skeletal definition
		void init(T3DMesh<float>* pMesh, bool CopyAnimationData = true);
		void update(float FPSScale);
		void clear(void);

		void addAnimationData(T3DMesh<float>::SkeletalAnimation* pAnimation);

		Animation* createAnimation(int32_t AnimationID, float Speed, float Offset);
		void destroyAnimation(Animation* pAnim);
		void applyAnimation(Animation* pAnim, bool UpdateUBO = true);

		UBOBoneData* ubo(void);

		T3DMesh<float>::SkeletalAnimation* animation(uint32_t ID);
		uint32_t animationCount(void)const;

		GLShader* shadowPassShader(void);

	protected:

		struct Joint {
			int32_t ID;
			std::string Name;
			Eigen::Matrix4f OffsetMatrix;
			Eigen::Vector3f LocalPosition;
			Eigen::Quaternionf LocalRotation;
			Eigen::Vector3f LocalScale;
			Eigen::Matrix4f SkinningMatrix;

			Joint* pParent;
			std::vector<Joint*> Children;
		};

		void transformSkeleton(Joint* pJoint, Eigen::Matrix4f ParentTransform);
		int32_t jointIDFromName(std::string JointName);

		Joint* m_pRoot;
		std::vector<Joint*> m_Joints;
			
		std::vector<T3DMesh<float>::SkeletalAnimation*> m_SkeletalAnimations; // available animations for this skeleton
		std::vector<Animation*> m_ActiveAnimations;

		UBOBoneData m_UBO;
		GLShader *m_pShadowPassShader;
		ShaderCode* m_pShadowPassVSCode;
		ShaderCode* m_pShadowPassFSCode;

	};//SkeletalAnimationController

}//name space

#endif 