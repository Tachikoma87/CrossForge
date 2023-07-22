#include "HingeLimits.h"

using namespace Eigen;

namespace CForge {

	HingeLimits::HingeLimits(const Eigen::Quaternionf LocalRestPose, const Eigen::Vector3f HingeAxis, const Eigen::Vector3f DefaultDir, const float MinRad, const float MaxRad) : JointLimits("HingeLimits") {
		m_HingeAxis = HingeAxis;
		m_HingeAxisInParentSpace = LocalRestPose * HingeAxis;
		m_DefaultDir = DefaultDir;
		m_RestDir = LocalRestPose * DefaultDir;
		m_MinRad = MinRad;
		m_MaxRad = MaxRad;
	}//Constructor

	HingeLimits::~HingeLimits(void) {

	}//Destructor

	Eigen::Quaternionf HingeLimits::constrain(const Eigen::Quaternionf Rotation) {
		// enforce rotation around hinge axis
		Quaternionf AxisConstraint;
		AxisConstraint.setFromTwoVectors(Rotation * m_HingeAxis, m_HingeAxisInParentSpace);
		Quaternionf ConstrainedRotation = AxisConstraint * Rotation;

		// compute new angle of joint relative to its rest position
		Vector3f NewDir = ConstrainedRotation * m_DefaultDir;
		NewDir.normalize();

		Vector3f Cross = m_RestDir.cross(NewDir);
		float Dot = m_RestDir.dot(NewDir);
		float AngleToRest = std::atan2(Cross.dot(m_HingeAxisInParentSpace), Dot); // angle from RestDir to NewDir in radians

		// enforce angle limits by adding a rotation that moves the joint back into the allowed range of motion
		float Diff = 0.0f;
		if (AngleToRest > m_MaxRad) Diff = m_MaxRad - AngleToRest;
		if (AngleToRest < m_MinRad) Diff = m_MinRad - AngleToRest;


		if (std::abs(Diff) > 1e-6f) {
			Quaternionf BackRotation = Quaternionf(AngleAxisf(Diff, m_HingeAxisInParentSpace));
			ConstrainedRotation = BackRotation * ConstrainedRotation;
		}

		ConstrainedRotation.normalize();

		return ConstrainedRotation;
	}//constrain

}