#ifndef __CFORGE_SWINGXYTWISTZLIMITS_H__
#define __CFORGE_SWINGXYTWISTZLIMITS_H__

#include "JointLimits.h"

namespace CForge {

	class SwingXYTwistZLimits : public JointLimits {
	public:
		SwingXYTwistZLimits(const Eigen::Quaternionf LocalRestPose, const float MinXSwing, const float MaxXSwing, const float MinYSwing, const float MaxYSwing, const float MinTwist, const float MaxTwist);

		~SwingXYTwistZLimits(void);

		virtual Eigen::Quaternionf constrain(const Eigen::Quaternionf Rotation) override;

	private:
		Eigen::Quaternionf m_LocalRestPose; // copy of LocalRotation at time of initialization
		float m_MinXSwing; // angle in radians
		float m_MaxXSwing; // angle in radians
		float m_MinYSwing; // angle in radians
		float m_MaxYSwing; // angle in radians
		float m_MinTwist;  // angle in radians
		float m_MaxTwist;  // angle in radians
	};

}

#endif