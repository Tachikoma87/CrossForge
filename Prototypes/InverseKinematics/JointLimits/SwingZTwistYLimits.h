#ifndef __CFORGE_SWINGZTWISTYLIMITS_H__
#define __CFORGE_SWINGZTWISTYLIMITS_H__

#include "JointLimits.h"

namespace CForge {

	class SwingZTwistYLimits : public JointLimits {
	public:
		SwingZTwistYLimits(const Eigen::Quaternionf LocalRestPose, const float MinSwing, const float MaxSwing, const float MinTwist, const float MaxTwist);

		~SwingZTwistYLimits(void);

		virtual Eigen::Quaternionf constrain(const Eigen::Quaternionf Rotation) override;

	private:
		Eigen::Quaternionf m_LocalRestPose; // copy of LocalRotation at time of initialization
		float m_MinSwing; // angle in radians
		float m_MaxSwing; // angle in radians
		float m_MinTwist; // angle in radians
		float m_MaxTwist; // angle in radians
	};

}

#endif