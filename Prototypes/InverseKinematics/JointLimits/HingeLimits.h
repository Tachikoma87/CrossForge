#ifndef __CFORGE_HINGELIMITS_H__
#define __CFORGE_HINGELIMITS_H__

#include "JointLimits.h"

namespace CForge {
	class HingeLimits : public JointLimits {
	public:
		HingeLimits(const Eigen::Quaternionf LocalRestPose, const Eigen::Vector3f HingeAxis, const Eigen::Vector3f DefaultDir, const float MinRad, const float MaxRad);
		~HingeLimits(void);

		virtual Eigen::Quaternionf constrain(const Eigen::Quaternionf Rotation) override;

	private:
		Eigen::Vector3f m_HingeAxis;
		Eigen::Vector3f m_HingeAxisInParentSpace;
		Eigen::Vector3f m_DefaultDir;
		Eigen::Vector3f m_RestDir;
		float m_MinRad;
		float m_MaxRad;
	};
}

#endif