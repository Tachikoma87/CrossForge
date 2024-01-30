/*****************************************************************************\
*                                                                           *
* File(s): MorphTargetAnimationController.h and MorphTargetAnimationController.cpp                        *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_MORPHTARGETANIMATIONCONTROLLER_H__
#define __CFORGE_MORPHTARGETANIMATIONCONTROLLER_H__

#include "../../AssetIO/T3DMesh.hpp"
#include "../UniformBufferObjects/UBOMorphTargetData.h"

namespace CForge {
	class CFORGE_API MorphTargetAnimationController: public CForgeObject {
	public:
		struct MorphTarget {
			int32_t ID;
			std::string Name;
		};//MorphTarget

		struct AnimationSequence {
			int32_t ID;
			std::string Name;
			std::vector<int32_t> Targets; // sequence of targets
			std::vector<Eigen::Vector3f> Parameters; // begin strength, end strength, duration in seconds

			void clear(void) {
				ID = -1;
				Name = "";
				Targets.clear();
				Parameters.clear();
			}
		};//AnimationSequence

		struct ActiveAnimation {
			int32_t ID;
			float Speed; ///< Progression speed
			float t;	///< current stamp
			int32_t SequenceID;
			int32_t CurrentSquenceIndex;
			uint64_t SequenceStartTimestamp;
			bool Finished;
		};

		MorphTargetAnimationController(void);
		~MorphTargetAnimationController(void);

		void init(T3DMesh<float>* pMesh);
		void clear(void);
		void update(float FPSScale);

		int32_t addAnimationSequence(AnimationSequence* pSequence);
		ActiveAnimation* play(int32_t SequenceID, float Speed);

		void apply(std::vector<ActiveAnimation*>* pAnims, UBOMorphTargetData* pUBO);
		bool valid(ActiveAnimation* pAnim);

		uint32_t morphTargetCount(void)const;
		MorphTarget* morphTarget(int32_t ID);

		uint32_t animationSequenceCount(void)const;
		AnimationSequence *animationSequence(int32_t ID);

	protected:

		void progress(ActiveAnimation* pAnim, float Scale);

		std::vector<MorphTarget*> m_MorphTargets;
		std::vector<AnimationSequence*> m_AnimationSequences;

		std::vector<ActiveAnimation*> m_ActiveAnimations;
	};//MorphTargetAnimationController
}//name space

#endif 