/*****************************************************************************\
*                                                                           *
* File(s): PointLight.h and PointLight.cpp                                *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_POINTLIGHT_H__
#define __CFORGE_POINTLIGHT_H__

#include "ILight.h"

namespace CForge {

	/**
	* \brief The point light. Placed somewhere in the world, casts light in all direction and has attenuation property.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API PointLight : public ILight {
	public:
		PointLight(void);
		~PointLight(void);

		void init(const Eigen::Vector3f Pos, const Eigen::Vector3f Dir, const Eigen::Vector3f Color, float Intensity, Eigen::Vector3f Attenuation);

		void attenuation(Eigen::Vector3f Attenuation);
		Eigen::Vector3f attenuation(void)const;

	protected:
		Eigen::Vector3f m_Attenuation;

	};//PointLight

}//name space

#endif 