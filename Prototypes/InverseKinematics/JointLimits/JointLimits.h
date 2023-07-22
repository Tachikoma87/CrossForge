#ifndef __CFORGE_JOINTLIMITS_H__
#define __CFORGE_JOINTLIMITS_H__

#include "crossforge/Core/CForgeObject.h"

namespace CForge {

	class JointLimits : public CForgeObject {
	public:
		JointLimits(const std::string ClassName);
		virtual ~JointLimits(void);
		virtual Eigen::Quaternionf constrain(const Eigen::Quaternionf Rotation) = 0;

	protected:
		void decomposeSwingTwist(const Eigen::Quaternionf& Rotation, const Eigen::Vector3f& TwistAxis, Eigen::Quaternionf& Swing, Eigen::Quaternionf& Twist);
		void decomposeSwingYZTwistX(const Eigen::Quaternionf& Rotation, Eigen::Quaternionf& Swing, Eigen::Quaternionf& Twist);
		void decomposeSwingXZTwistY(const Eigen::Quaternionf& Rotation, Eigen::Quaternionf& Swing, Eigen::Quaternionf& Twist);
		void decomposeSwingXYTwistZ(const Eigen::Quaternionf& Rotation, Eigen::Quaternionf& Swing, Eigen::Quaternionf& Twist);
		Eigen::Vector3f quatLog(Eigen::Quaternionf Q);
		Eigen::Quaternionf quatExp(Eigen::Vector3f V);
		Eigen::Quaternionf quatAbs(Eigen::Quaternionf Q);		
		Eigen::Vector2f intersectionSegmentEllipse(float A, float B, Eigen::Vector2f Query);
		float squared(float Val);
		float robustLength(float V0, float V1);
		float root(float r0, float z0, float z1, float g);
		void closestPointOnEllipse(float e0, float e1, float y0, float y1, float& x0, float& x1);

		float m_MaxItRootFinding;
	};
}

#endif