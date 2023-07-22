#include "SwingXZTwistYLimits.h"
#include <crossforge/Math/CForgeMath.h>

using namespace Eigen;

namespace CForge {

	SwingXZTwistYLimits::SwingXZTwistYLimits(const Eigen::Quaternionf LocalRestPose, const float MinXSwing, const float MaxXSwing, const float MinZSwing, const float MaxZSwing, const float MinTwist, const float MaxTwist) : JointLimits("SwingXZTwistYLimits") {				
		if (MinXSwing < -EIGEN_PI || MinXSwing >= 0.0f) throw CForgeExcept("MinXSwing is outside of the allowed range of [-pi, 0)!");
		if (MaxXSwing <= 0.0f || MaxXSwing > EIGEN_PI) throw CForgeExcept("MaxXSwing is outside of the allowed range of (0, pi]!");

		if (MinZSwing < -EIGEN_PI || MinZSwing >= 0.0f) throw CForgeExcept("MinZSwing is outside of the allowed range of [-pi, 0)!");
		if (MaxZSwing < 0.0f || MaxZSwing > EIGEN_PI) throw CForgeExcept("MaxZSwing is outside of the allowed range of (0, pi]!");

		if (MinTwist < -EIGEN_PI || MinTwist > 0.0f) throw CForgeExcept("MinTwist is outside of the allowed range of [-pi, 0]!");
		if (MaxTwist < 0.0f || MaxTwist > EIGEN_PI) throw CForgeExcept("MaxTwist is outside of the allowed range of [0, pi]!");

		m_LocalRestPose = LocalRestPose;
		m_MinXSwing = std::abs(MinXSwing);
		m_MaxXSwing = MaxXSwing;
		m_MinZSwing = std::abs(MinZSwing);
		m_MaxZSwing = MaxZSwing;
		m_MinTwist = MinTwist;
		m_MaxTwist = MaxTwist;
	}//Constructor

	SwingXZTwistYLimits::~SwingXZTwistYLimits(void) {

	}//Destructor

	Quaternionf SwingXZTwistYLimits::constrain(const Quaternionf Rotation) {
		// remove rest pose from current rotation so that limits work properly (rotations must be relative to zero rotation position of bone)
		Quaternionf ToConstrain = quatAbs(m_LocalRestPose.conjugate() * Rotation);

		Quaternionf Swing, Twist;
		decomposeSwingXZTwistY(ToConstrain, Swing, Twist);

		// limit swing rotation using ellipses
		AngleAxisf SwingAA = AngleAxisf(Swing);
		float A = std::signbit(SwingAA.axis().x()) ? m_MinXSwing : m_MaxXSwing; // horizontal half extent
		float B = std::signbit(SwingAA.axis().z()) ? m_MinZSwing : m_MaxZSwing; // vertical half extent
		float QueryX = SwingAA.angle() * SwingAA.axis().x(); // query point x -> coordinate on horizontal axis in 2D (x-component of scaled angle-axis-vector)
		float QueryZ = SwingAA.angle() * SwingAA.axis().z(); // query point z -> coordinate on vertical axis in 2D (z-component of scaled angle-axis-vector)		
		bool OutsideEllipse = ((QueryX * QueryX) / (A * A)) + ((QueryZ * QueryZ) / (B * B)) > 1.0f;
		
		if (OutsideEllipse) {
			// following section 2 of "Distance from a Point to an Ellipse, an Ellipsoid, or a Hyperellipsoid":
			// -> the search for a closest point on the ellipse will always be executed in the first quadrant in 2D 
			// -> the horizontal extent of the ellipse has to be equal or larger than the vertical extent (A >= B)

			bool XIsNegative = std::signbit(QueryX);
			bool ZIsNegative = std::signbit(QueryZ);
			bool SwapAxes = B > A;

			// transform the query point to the first quadrant by changing the signs of its components
			if (XIsNegative) QueryX = -QueryX;
			if (ZIsNegative) QueryZ = -QueryZ;

			// swap axes if B > A so we can use the closest-point algorithm of "Distance from a Point to an Ellipse, an Ellipsoid, or a Hyperellipsoid"
			if (SwapAxes) {
				std::swap(QueryX, QueryZ);
				std::swap(A, B);
			}

			// compute the closest point on the ellipse in the first quadrant
			float ClosestX, ClosestZ;
			closestPointOnEllipse(A, B, QueryX, QueryZ, ClosestX, ClosestZ);

			// swap axes back if necessary
			if (SwapAxes) std::swap(ClosestX, ClosestZ);

			// undo the previous sign changes to obtain the result in the original quadrant of the query point
			if (XIsNegative) ClosestX = -ClosestX;
			if (ZIsNegative) ClosestZ = -ClosestZ;

			// apply
			Vector3f ConstrainedSwingSAA = Vector3f(ClosestX, 0.0f, ClosestZ); // constrained version of scaled angle-axis vector of swing
			float NewSwingAngle = ConstrainedSwingSAA.norm();
			Vector3f NewSwingAxis = ConstrainedSwingSAA.normalized();
			Swing = Quaternionf(AngleAxisf(NewSwingAngle, NewSwingAxis));
		}

		// limit twist rotation
		AngleAxisf TwistAA = AngleAxisf(Twist);
		float TwistAngle = std::clamp(TwistAA.angle() * TwistAA.axis().y(), m_MinTwist, m_MaxTwist);
		Twist = Quaternionf(AngleAxisf(TwistAngle, Vector3f::UnitY()));

		// recompose constrained rotation, reapply rest pose, return
		return m_LocalRestPose * (Swing * Twist);
	}//constrain

}