#include "CForgeMath.h"

using namespace Eigen;

namespace CForge {

	uint64_t CForgeMath::m_RndState = 88172645463325252ull;

	uint64_t CForgeMath::rand(void) {
		m_RndState ^= m_RndState << 13;
		m_RndState ^= m_RndState >> 7;
		m_RndState ^= m_RndState << 17;
		return m_RndState / 2ull;
	}//rand

	void CForgeMath::randSeed(uint64_t Seed) {
		m_RndState = Seed;
	}//randSeed


	Eigen::Matrix4f CForgeMath::perspectiveProjection(uint32_t Width, uint32_t Height, float FieldOfView, float Near, float Far) {
		if (Near >= Far) throw CForgeExcept("Near plane further away than far plane!");
		if (Width == 0 || Height == 0) throw CForgeExcept("Viewport width and/or viewport height specified with 0!");


		Eigen::Matrix4f Rval = Eigen::Matrix4f::Identity();

		float Aspect = float(Width) / float(Height);
		float Theta = FieldOfView * 0.5f;
		float Range = Far - Near;
		float InvTan = 1.0 / std::tan(Theta);

		Rval(0, 0) = InvTan / Aspect;
		Rval(1, 1) = InvTan;
		Rval(2, 2) = -(Near + Far) / Range;
		Rval(3, 2) = -1.0f;
		Rval(2, 3) = -2.0f * Near * Far / Range;
		Rval(3, 3) = 0.0f;

		return Rval;
	}//perspective

	// same as glFrustum
	Eigen::Matrix4f CForgeMath::perspectiveProjection(float Left, float Right, float Bottom, float Top, float Near, float Far) {
		Eigen::Matrix4f Rval = Matrix4f::Zero();
		Rval(0, 0) = 2.0f * Near / (Right - Left);
		Rval(0, 2) = (Right + Left) / (Right - Left);
		Rval(1, 1) = 2.0f * Near / (Top - Bottom);
		Rval(1, 2) = (Top + Bottom) / (Top - Bottom);
		Rval(2, 2) = -(Far + Near) / (Far - Near);
		Rval(2, 3) = (-2.0f * Far * Near) / (Far - Near);
		Rval(3, 2) = -1.0f;
		return Rval;
	}//persepctiveProjection

	void CForgeMath::asymmetricFrusti(uint32_t Width, uint32_t Height, float Near, float Far, float FOV, float FocalLength, float EyeSep, Eigen::Matrix4f* pLeftEye, Eigen::Matrix4f* pRightEye) {
		float Aspect = Width / (float)Height;
		float YnMax = Near * std::tan(FOV / 2.0f);
		float Delta = 0.5f * EyeSep * Near / FocalLength;

		float Top = YnMax;
		float Bottom = -YnMax;
		float Left = -(Aspect * YnMax) - Delta;
		float Right = (Aspect * YnMax) - Delta;

		if (nullptr != pRightEye) (*pRightEye) = perspectiveProjection(Left, Right, Bottom, Top, Near, Far);

		Left = -(Aspect * YnMax) + Delta;
		Right = (Aspect * YnMax) + Delta;
		if (nullptr != pLeftEye) (*pLeftEye) = perspectiveProjection(Left, Right, Bottom, Top, Near, Far);

	}//asymmetricFrusti

	Eigen::Matrix4f CForgeMath::orthographicProjection(float Left, float Right, float Bottom, float Top, float Near, float Far) {
		Eigen::Matrix4f Rval = Eigen::Matrix4f::Identity();
		Rval(0, 0) = 2.0f / (Right - Left);
		Rval(0, 3) = -(Right + Left) / (Right - Left);
		Rval(1, 1) = 2.0f / (Top - Bottom);
		Rval(1, 3) = -(Top + Bottom) / (Top - Bottom);
		Rval(2, 2) = -2.0f / (Far - Near);
		Rval(2, 3) = -(Far + Near) / (Far - Near);

		return Rval;
	}//orthographic

	Eigen::Matrix4f CForgeMath::orthographicProjection(float Right, float Top, float Near, float Far) {
		Eigen::Matrix4f Rval = Eigen::Matrix4f::Identity();

		Rval(0, 0) = 1.0f / Right;
		Rval(1, 1) = 1.0f / Top;
		Rval(2, 2) = -2.0f / (Far - Near);
		Rval(2, 3) = -(Far + Near) / (Far - Near);

		return Rval;
	}//orthographic

	Eigen::Matrix4f CForgeMath::lookAt(Eigen::Vector3f Position, Eigen::Vector3f Target, Eigen::Vector3f Up) {
		Eigen::Matrix4f Rval = Eigen::Matrix4f::Identity();

		Eigen::Vector3f f = Target - Position;
		Eigen::Vector3f s = f.cross(Up);
		f.normalize();
		s.normalize();
		Eigen::Vector3f u = s.cross(f);

		Rval(0, 0) = s.x();
		Rval(0, 1) = s.y();
		Rval(0, 2) = s.z();
		Rval(1, 0) = u.x();
		Rval(1, 1) = u.y();
		Rval(1, 2) = u.z();
		Rval(2, 0) = -f.x();
		Rval(2, 1) = -f.y();
		Rval(2, 2) = -f.z();

		Eigen::Vector4f V = Rval * Eigen::Vector4f(-Position.x(), -Position.y(), -Position.z(), 1.0f);
		Rval(0, 3) = V.x();
		Rval(1, 3) = V.y();
		Rval(2, 3) = V.z();

		return Rval;
	}//lookAt

	Eigen::Matrix4f CForgeMath::rotationMatrix(Eigen::Quaternionf Rot) {
		Matrix4f Rval = Matrix4f::Identity();
		const Matrix3f R = Rot.toRotationMatrix();
		for (uint8_t i = 0; i < 3; ++i) {
			for (uint8_t k = 0; k < 3; ++k) {
				Rval(i, k) = R(i, k);
			}
		}
		return Rval;
	}//rotationMatrix

	Eigen::Matrix4f CForgeMath::translationMatrix(Eigen::Vector3f Trans) {
		Matrix4f Rval = Matrix4f::Identity();
		Rval(0, 3) = Trans.x();
		Rval(1, 3) = Trans.y();
		Rval(2, 3) = Trans.z();
		return Rval;
	}//translationMatrix

	Eigen::Matrix4f CForgeMath::scaleMatrix(Eigen::Vector3f Scale) {
		Matrix4f Rval = Matrix4f::Identity();
		Rval(0, 0) = Scale.x();
		Rval(1, 1) = Scale.y();
		Rval(2, 2) = Scale.z();
		Rval(3, 3) = 1.0f;
		return Rval;
	}//scaleMatrix

	// thanks to: ChatGPT
	Eigen::Matrix3f CForgeMath::alignVectors(const Eigen::Vector3f Source, const Eigen::Vector3f Target) {
		const Vector3f a = Source;
		const Vector3f b = Target;
		const float ADotB = a.dot(b);

		Matrix3f Rval;
		// if both vectors point in the same direction, they are already aligned
		if (ADotB > 0.999f && ADotB < 1.0001f) {
			Rval = Matrix3f::Identity();
		}
		// if they point in opposite direction, we rotate 180 degrees about a perpendicular axis
		else if (ADotB < -0.9999f && ADotB > -1.0001f) {
			Rval = AngleAxisf(CForgeMath::degToRad(180.0f), Eigen::Vector3f(a.y(), a.z(), a.x()));
		}
		else {
			const Vector3f v = a.cross(b);
			Rval = AngleAxisf(std::acos(ADotB), v);
		}
		return Rval;
	}//alignVectors

	Eigen::Vector3f CForgeMath::equirectangularMapping(const Vector3f Pos) {
		Vector3f Rval;
		Rval.x() = std::atan2(Pos.x(), -Pos.z()) / (2.0f * EIGEN_PI) + 0.5f;
		Rval.y() = Pos.y() * 0.5f + 0.5f;
		Rval.z() = 0.0f;
		return Rval;
	}//equirectangularMapping

	Eigen::Vector3f CForgeMath::equalAreaMapping(const Vector3f Pos) {
		Vector3f Rval;
		Rval.x() = (std::atan2(Pos.x(), -Pos.z()) / EIGEN_PI + 1.0f) / 2.0f;
		Rval.y() = std::asin(Pos.y()) / EIGEN_PI + 0.5f;
		Rval.z() = 0.0f;
		return Rval;
	}//equalAreaMapping


	CForgeMath::CForgeMath(void): CForgeObject("CForgeMath") {

	}//Constructor

	CForgeMath::~CForgeMath(void) {

	}//Destructor

}//name space