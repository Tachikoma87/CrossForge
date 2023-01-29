/*****************************************************************************\
*                                                                           *
* File(s): CForgeMath.h and CForgeMath.cpp                               *
*                                                                           *
* Content: Utility methods for math stuff.                   *
*                *
*                               *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_CFORGEMATH_H__
#define __CFORGE_CFORGEMATH_H__

#include <chrono>
#include "../Core/CForgeObject.h"

namespace CForge {
	class CFORGE_API CForgeMath : public CForgeObject {
	public:
		
		template<typename T>
		static T randRange(T Lower, T Upper) {
			long double R = (long double)(rand()) / (long double)(randMax());
			long double Temp = R * (Upper - Lower);
			return T(Lower + Temp);
			//return T(Lower + R * (Upper - Lower));
		}//randRange

		static uint64_t rand(void);
		static void randSeed(uint64_t Seed);

		static uint64_t randMax(void) {
			return std::numeric_limits<uint64_t>::max() / 2ull;
		}//randMax

		template<typename T>
		static T degToRad(T Deg) {
			return Deg * T(EIGEN_PI) / T(180);
		}//degToRad

		template<typename T>
		static T radToDeg(T Rad) {
			return Rad * T(180) / T(EIGEN_PI);
		}//radToDeg

		static Eigen::Matrix4f perspectiveProjection(uint32_t Width, uint32_t Height, float FieldOfView, float Near, float Far);
		static Eigen::Matrix4f perspectiveProjection(float Left, float Right, float Bottom, float Top, float Near, float Far);
		static Eigen::Matrix4f orthographicProjection(float Left, float Right, float Bottom, float Top, float Near, float Far);
		static Eigen::Matrix4f orthographicProjection(float Right, float Top, float Near, float Far);
		static Eigen::Matrix4f lookAt(Eigen::Vector3f Position, Eigen::Vector3f Target, Eigen::Vector3f Up = Eigen::Vector3f::UnitY());
		static void asymmetricFrusti(uint32_t Width, uint32_t Height, float Near, float Far, float FOV, float FocalLength, float EyeSep, Eigen::Matrix4f* pLeftEye, Eigen::Matrix4f* pRightEye);

		static Eigen::Matrix4f rotationMatrix(Eigen::Quaternionf Rot);
		static Eigen::Matrix4f translationMatrix(Eigen::Vector3f Trans);
		static Eigen::Matrix4f scaleMatrix(Eigen::Vector3f Scale);

		static Eigen::Matrix3f alignVectors(const Eigen::Vector3f Source, const Eigen::Vector3f Target);

		static Eigen::Vector3f equirectangularMapping(const Eigen::Vector3f Pos);
		static Eigen::Vector3f equalAreaMapping(const Eigen::Vector3f Pos);

		CForgeMath(void);
		~CForgeMath(void);

	protected:
		static uint64_t m_RndState; 
	};//CForgeMath

}//name space

#endif 