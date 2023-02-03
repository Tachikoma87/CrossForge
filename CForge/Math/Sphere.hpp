/*****************************************************************************\
*                                                                           *
* File(s): Sphere.hpp                               *
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
#ifndef __CFORGE_SPHERE_HPP__
#define __CFORGE_SPHERE_HPP__

#include "../Core/CForgeObject.h"

namespace CForge {
	class Sphere {
	public:
		Sphere(void){
			m_Center = Eigen::Vector3f::Zero();
			m_Radius = 0.0f;
		}//Constructor

		~Sphere(void) {
			clear();
		}//Destructor

		void init(const Eigen::Vector3f Center, const float Radius) {
			m_Center = Center;
			m_Radius = Radius;
		}//initialize

		void clear(void) {
			m_Center = Eigen::Vector3f::Zero();
			m_Radius = 0.0f;
		}//clear

		void release(void) {
			delete this;
		}//release

		Eigen::Vector3f center(void)const {
			return m_Center;
		}//center

		float radius(void)const {
			return m_Radius;
		}//radius

		void center(const Eigen::Vector3f Center) {
			m_Center = Center;
		}//center

		void radius(const float Radius) {
			m_Radius = Radius;
		}//radius

	protected:
		Eigen::Vector3f m_Center;
		float m_Radius;
	};//Sphere

}//name space


#endif 

