#include "SwingXTwistYLimits.h"
#include <crossforge/Math/CForgeMath.h>

using namespace Eigen;

namespace CForge {

	SwingXTwistYLimits::SwingXTwistYLimits(const Eigen::Quaternionf LocalRestPose, const float MinSwing, const float MaxSwing, const float MinTwist, const float MaxTwist) : JointLimits("SwingXTwistYLimits") {		
		if (MinSwing < -EIGEN_PI || MinSwing > 0.0f) throw CForgeExcept("MinSwing outside allowed range [-pi, 0]!");
		if (MaxSwing < 0.0f || MaxSwing > EIGEN_PI) throw CForgeExcept("MaxSwing outside allowed range [0, pi]!");

		if (MinTwist < -EIGEN_PI || MinTwist > 0.0f) throw CForgeExcept("MinTwist outside allowed range [-pi, 0]!");
		if (MaxTwist < 0.0f || MaxTwist > EIGEN_PI) throw CForgeExcept("MaxTwist outside allowed range [0, pi]!");

		m_LocalRestPose = LocalRestPose;
		m_MinSwing = MinSwing;
		m_MaxSwing = MaxSwing;
		m_MinTwist = MinTwist;
		m_MaxTwist = MaxTwist;
	}//Constructor

	SwingXTwistYLimits::~SwingXTwistYLimits(void) {

	}//Destructor

	Quaternionf SwingXTwistYLimits::constrain(const Quaternionf Rotation) {
		// remove rest pose from current rotation so that limits work properly (rotations must be relative to zero rotation position of bone)
		Quaternionf ToConstrain = quatAbs(m_LocalRestPose.conjugate() * Rotation);

		Quaternionf Swing, Twist;
		decomposeSwingXZTwistY(ToConstrain, Swing, Twist);

		// limit swing rotation
		AngleAxisf SwingAA = AngleAxisf(Swing);
		float SwingAngle = std::clamp(SwingAA.angle() * SwingAA.axis().x(), m_MinSwing, m_MaxSwing);
		Swing = Quaternionf(AngleAxisf(SwingAngle, Vector3f::UnitX()));

		// limit twist rotation
		AngleAxisf TwistAA = AngleAxisf(Twist);
		float TwistAngle = std::clamp(TwistAA.angle() * TwistAA.axis().y(), m_MinTwist, m_MaxTwist);
		Twist = Quaternionf(AngleAxisf(TwistAngle, Vector3f::UnitY()));

		// recompose constrained rotation, reapply rest pose, return
		return m_LocalRestPose * (Swing * Twist);
	}//constrain
	
}