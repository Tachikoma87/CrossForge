#include "JointLimits.h"
#include <crossforge/Math/CForgeMath.h>

using namespace Eigen;

namespace CForge {

	JointLimits::JointLimits(const std::string ClassName) : CForgeObject("JointLimits::" + ClassName) {
		m_MaxItRootFinding = std::numeric_limits<float>::digits - std::numeric_limits<float>::min_exponent;
	}//Constructor

	JointLimits::~JointLimits(void) {

	}//Destructor

	void JointLimits::decomposeSwingTwist(const Quaternionf& Rotation, const Vector3f& TwistAxis, Quaternionf& Swing, Quaternionf& Twist) {
		// From: https://stackoverflow.com/questions/3684269/component-of-a-quaternion-rotation-around-an-axis -> answer of user 'minorlogic' on Mar 14, 2014

		Vector3f RA = Vector3f(Rotation.x(), Rotation.y(), Rotation.z());
		Vector3f ProjRA = (RA.dot(TwistAxis)) * TwistAxis;
		Twist = Quaternionf(Rotation.w(), ProjRA.x(), ProjRA.y(), ProjRA.z());

		// singularity: rotation by 180 degrees -> twist could be anything, choose a twist of 0 degree
		// 
		// (according to https://stackoverflow.com/questions/3684269/component-of-a-quaternion-rotation-around-an-axis this case can be checked by testing if the
		// magnitude of the unnormalized twist rotation is close to zero)
		if (Twist.norm() < 1e-8f)
			Twist = Quaternionf::Identity();

		Twist.normalize();

		Swing = Rotation * Twist.conjugate();
		Swing.normalize();
	}//decomposeSwingTwist

	void JointLimits::decomposeSwingYZTwistX(const Eigen::Quaternionf& Rotation, Eigen::Quaternionf& Swing, Eigen::Quaternionf& Twist) {
		// Modified from: [Design and Implementation of a Mobile Sensor System for Human Posture Tracking] (PhD dissertation)

		float Sq = Rotation.w() * Rotation.w() + Rotation.x() * Rotation.x();

		if (Sq > 0.0f) {
			float InvSqrt = 1.0f / std::sqrt(Sq);
			Twist = Quaternionf(Rotation.w() * InvSqrt, Rotation.x() * InvSqrt, 0.0f, 0.0f);
			//Twist.normalize();
		}
		else {
			// singularity: if both Rotation.w() and Rotation.x() are (very close to) zero, the quaternion corresponds to a rotation of 180 degrees in the YZ-plane
			// -> twist angle can be anything
			Twist = Quaternionf::Identity(); //Twist = Quaternionf(AngleAxisf(CForgeMath::degToRad(0.0f), Vector3f::UnitX()));
		}

		Swing = Rotation * Twist.conjugate();
		//Swing.normalize();
	}//decomposeSwingYZTwistX

	void JointLimits::decomposeSwingXZTwistY(const Eigen::Quaternionf& Rotation, Eigen::Quaternionf& Swing, Eigen::Quaternionf& Twist) {
		// Modified from: [Design and Implementation of a Mobile Sensor System for Human Posture Tracking] (PhD dissertation)

		float Sq = Rotation.w() * Rotation.w() + Rotation.y() * Rotation.y();

		if (Sq > 0.0f) {
			float InvSqrt = 1.0f / std::sqrt(Sq);
			Twist = Quaternionf(Rotation.w() * InvSqrt, 0.0f, Rotation.y() * InvSqrt, 0.0f);
			//Twist.normalize();
		}
		else {
			// singularity: if both Rotation.w() and Rotation.y() are (very close to) zero, the quaternion corresponds to a rotation of 180 degrees in the XZ-plane
			// -> twist angle can be anything
			Twist = Quaternionf::Identity(); //Twist = Quaternionf(AngleAxisf(CForgeMath::degToRad(0.0f), Vector3f::UnitY()));
		}

		Swing = Rotation * Twist.conjugate();
		//Swing.normalize();
	}//decomposeSwingXZTwistY

	void JointLimits::decomposeSwingXYTwistZ(const Eigen::Quaternionf& Rotation, Eigen::Quaternionf& Swing, Eigen::Quaternionf& Twist) {
		// From: [Design and Implementation of a Mobile Sensor System for Human Posture Tracking] (PhD dissertation)

		float Sq = Rotation.w() * Rotation.w() + Rotation.z() * Rotation.z();

		if (Sq > 0.0f) {
			float InvSqrt = 1.0f / std::sqrt(Sq);
			Twist = Quaternionf(Rotation.w() * InvSqrt, 0.0f, 0.0f, Rotation.z() * InvSqrt);
			//Twist.normalize();
		}
		else {
			// singularity: if both Rotation.w() and Rotation.z() are (very close to) zero, the quaternion corresponds to a rotation of 180 degrees in the XY-plane
			// -> twist angle can be anything
			Twist = Quaternionf::Identity(); //Twist = Quaternionf(AngleAxisf(CForgeMath::degToRad(0.0f), Vector3f::UnitZ()));
		}

		Swing = Rotation * Twist.conjugate();
		//Swing.normalize();
	}//decomposeSwingXYTwistZ

	Eigen::Vector3f JointLimits::quatLog(Eigen::Quaternionf Q) {
		Vector3f V;

		float Length = Q.vec().norm();

		if (Length < 1e-8f) {
			V = Q.vec();
		}
		else {
			float HalfAngle = std::atan2f(Length, Q.w());
			V = HalfAngle * (Q.vec() / Length);
		}

		return V;
	}//quatLog

	Eigen::Quaternionf JointLimits::quatExp(Eigen::Vector3f V) {
		Quaternionf Q;
		
		float HalfAngle = V.stableNorm();
		
		if (HalfAngle < 1e-8f) {
			Q = Quaternionf(1.0f, V.x(), V.y(), V.z());
			Q.normalize();
		}
		else {
			float Cos = std::cos(HalfAngle);
			float Sin = std::sin(HalfAngle) / HalfAngle;
			Q = Quaternionf(Cos, Sin * V.x(), Sin * V.y(), Sin * V.z());
		}

		return Q;
	}//quatExp

	Eigen::Quaternionf JointLimits::quatAbs(Eigen::Quaternionf Q) {
		Quaternionf QAbs = (Q.w() < 0.0f) ? Quaternionf(-Q.w(), -Q.x(), -Q.y(), -Q.z()) : Q;
		return QAbs;
	}//quatAbs

	Vector2f JointLimits::intersectionSegmentEllipse(float A, float B, Vector2f Query) {
		float a = (Query.x() * Query.x()) / (A * A) + (Query.y() * Query.y()) / (B * B);
		float discr = 4 * a;
		float t = (std::sqrt(discr)) / (2 * a);

		return Vector2f(Query.x() * t, Query.y() * t);
	}//intersectionSegmentEllipse

	float JointLimits::squared(float Val) {
		return Val * Val;
	}//squared

	float JointLimits::robustLength(float V0, float V1) {
		float AbsV0 = std::abs(V0);
		float AbsV1 = std::abs(V1);
		float Length = (AbsV1 < AbsV0) ? AbsV0 * std::sqrt(1.0f + ((V1 / V0) * (V1 / V0))) : AbsV1 * std::sqrt(1.0f + ((V0 / V1) * (V0 / V1)));
		return Length;
	}//robustLength

	float JointLimits::root(float r0, float z0, float z1, float g) {
		float n0 = r0 * z0;
		float s0 = z1 - 1.0f;
		float s1 = (g < 0.0f ? 0.0f : robustLength(n0, z1) - 1.0f);
		float s = 0.0f;

		for (int32_t i = 0; i < m_MaxItRootFinding; ++i) {
			s = (s0 + s1) / 2.0f;

			if (s == s0 || s == s1) break;

			float ratio0 = n0 / (s + r0);
			float ratio1 = z1 / (s + 1.0f);

			g = squared(ratio0) + squared(ratio1) - 1.0f;

			if (g > 0.0f) {
				s0 = s;
			}
			else if (g < 0.0f) {
				s1 = s;
			}
			else {
				break;
			}
		}

		return s;
	}//root

	void JointLimits::closestPointOnEllipse(float e0, float e1, float y0, float y1, float& x0, float& x1) {
		// see: "Distance from a Point to an Ellipse, an Ellipsoid, or a Hyperellipsoid" - section 2.9
		if (y1 > 0.0f) {
			if (y0 > 0.0f) {
				float z0 = y0 / e0;
				float z1 = y1 / e1;
				float g = squared(z0) + squared(z1) - 1.0f;

				if (g != 0.0f) {
					float r0 = squared(e0 / e1);
					float sbar = root(r0, z0, z1, g);

					x0 = r0 * y0 / (sbar + r0);
					x1 = y1 / (sbar + 1.0f);
				}
				else {
					x0 = y0;
					x1 = y1;
				}
			}
			else { // y0 == 0
				x0 = 0.0f;
				x1 = e1;
			}
		}
		else { // y1 == 0
			float numer0 = e0 * y0;
			float denom0 = squared(e0) - squared(e1);

			if (numer0 < denom0) {
				float xde0 = numer0 / denom0;

				x0 = e0 * xde0;
				x1 = e1 * std::sqrt(1.0f - xde0 * xde0);
			}
			else {
				x0 = e0;
				x1 = 0.0f;
			}
		}
	}//closestPointOnEllipse
}