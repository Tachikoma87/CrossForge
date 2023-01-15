/*****************************************************************************\
*                                                                           *
* File(s): Plane.hpp                               *
*                                                                           *
* Content:                    *
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
#ifndef __CFORGE_PLANE_HPP__
#define __CFORGE_PLANE_HPP__

#include <CForge/Core/CForgeObject.h>

namespace CForge {
	class Plane {
	public:
		Plane(void) {

		}//Constructor

		~Plane(void) {

		}//Destructor

		void init(float Distance, Eigen::Vector3f Normal) {
			m_Normal = Normal;
			m_Distance = Distance;
		}//initialize

		void init(Eigen::Vector3f OnPoint, Eigen::Vector3f Normal) {
			m_Normal = Normal.normalized();
			m_Distance = OnPoint.dot(m_Normal);
		}//initialize

		Eigen::Vector3f normal(void)const {
			return m_Normal;
		}//normal

		void normal(Eigen::Vector3f Normal) {
			m_Normal = Normal;
		}//normal

		float distance(void)const {
			return m_Distance;
		}//distance

		void distance(float Distance) {
			m_Distance = Distance;
		}//distance

		float signedDistance(Eigen::Vector3f Point)const {
			return m_Normal.dot(Point) - m_Distance;
		}//distance

	protected:
		Eigen::Vector3f m_Normal;
		float m_Distance;
	};//Plane

}//name space

#endif 