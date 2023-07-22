#ifndef __CFORGE_SWINGXZTWISTYLIMITS_H__
#define __CFORGE_SWINGXZTWISTYLIMITS_H__

#include "JointLimits.h"

namespace CForge {

	class SwingXZTwistYLimits : public JointLimits {
	public:
		SwingXZTwistYLimits(const Eigen::Quaternionf LocalRestPose, const float MinXSwing, const float MaxXSwing, const float MinZSwing, const float MaxZSwing, const float MinTwist, const float MaxTwist);

		~SwingXZTwistYLimits(void);

		virtual Eigen::Quaternionf constrain(const Eigen::Quaternionf Rotation) override;

	private:
		Eigen::Quaternionf m_LocalRestPose; // copy of LocalRotation at time of initialization
		float m_MinXSwing; // angle in radians
		float m_MaxXSwing; // angle in radians
		float m_MinZSwing; // angle in radians
		float m_MaxZSwing; // angle in radians
		float m_MinTwist;  // angle in radians
		float m_MaxTwist;  // angle in radians
	};

}

#endif