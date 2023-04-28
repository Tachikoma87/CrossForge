/*****************************************************************************\
*                                                                           *
* File(s): Box.hpp                               *
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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_BOX_HPP__
#define __CFORGE_BOX_HPP__

#include "../Core/CForgeObject.h"

namespace CForge {
	class Box {
	public:
		Box(void) {
			m_Min = Eigen::Vector3f::Zero();
			m_Max = Eigen::Vector3f::Zero();
		}//Constructor

		~Box(void) {
			clear();
		}//Destructor

		void init(Eigen::Vector3f Min, Eigen::Vector3f Max) {
			m_Min = Min;
			m_Max = Max;
		}//initialize

		void clear(void) {
			m_Min = Eigen::Vector3f::Zero();
			m_Max = Eigen::Vector3f::Zero();
		}//clear

		void release(void) {
			delete this;
		}//release

		Eigen::Vector3f min(void)const {
			return m_Min;
		}//min

		Eigen::Vector3f max(void)const {
			return m_Max;
		}//max

		void min(const Eigen::Vector3f Min) {
			m_Min = Min;
		}//min

		void max(const Eigen::Vector3f Max) {
			m_Max = Max;
		}//max

		Eigen::Vector3f diagonal(void)const {
			return (m_Max - m_Min);
		}//diagonal

		Eigen::Vector3f center(void)const {
			return m_Min + 0.5f * diagonal();
		}//center

	protected:
		Eigen::Vector3f m_Min;
		Eigen::Vector3f m_Max;
	};//Box

}//name space


#endif 