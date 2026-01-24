/*****************************************************************************\
*                                                                           *
* File(s): AdaptiveSkeletalActor.h and AdaptiveSkeletalActor.cpp                               *
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
#ifndef __CFORGE_ADAPTIVESKELETALACTOR_H__
#define __CFORGE_ADAPTIVESKELETALACTOR_H__

#include <crossforge/Graphics/Actors/SkeletalActor.h>

namespace CForge {
	class AdaptiveSkeletalActor : public SkeletalActor {
	public:
		struct ControlParameters {
			int32_t VertexIDLeftHeel;
			int32_t VertexIDRightHeel;
			int32_t VertexIDLeftForefoot;
			int32_t VertexIDRightForefoot;

			int32_t RightAnkleJointID;
			int32_t LeftAnkleJointID;
			int32_t HipJointID; ///< Root node of the model.

			ControlParameters() {
				clear();
			}

			void clear() {
				VertexIDLeftHeel = -1;
				VertexIDRightHeel = -1;
				VertexIDLeftForefoot = -1;
				VertexIDRightForefoot = -1;

				RightAnkleJointID = -1;
				LeftAnkleJointID = -1;
				HipJointID = -1;
			}
		};


		AdaptiveSkeletalActor(void);
		~AdaptiveSkeletalActor(void);

		void init(T3DMesh<float>* pMesh, SkeletalAnimationController* pController, bool PrepareCPUSkinning = true) override;
		void render(RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) override;

		//Eigen::Vector3f getTransformedVertex(int32_t ID);

		void feetAlignment(bool Active);
		bool feetAlignment(void)const;

	protected:

		void alignFeetToGround(void);

		ControlParameters m_ControlParams;

		//SkeletalSkinning m_SkeletalSkinning;
		std::vector<SkeletalAnimationController::SkeletalJoint*> m_Joints;

		float m_GlobalYCorrection;
		bool m_FeetAlignmentToggle;

		float m_LastAdaptationAngle;
		float m_MaxAdaptationDelta;
		int32_t m_LastFootAdaptation; // 0 = none, 1 = left, 3 = right

	};//AdaptiveSkeletalActor
}

#endif 