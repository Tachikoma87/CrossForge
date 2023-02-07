/*****************************************************************************\
*                                                                           *
* File(s): SpotLight.h and SpotLight.cpp                                *
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
#ifndef __CFORGE_SPOTLIGHT_H__
#define __CFORGE_SPOTLIGHT_H__

#include "ILight.h"

namespace CForge {

	/**
	* \brief A spotlight casts light in a certain direction and within a specified cone.
	*
	* \todo Do full documentation
	*/
	class CFORGE_API SpotLight : public ILight {
	public:
		SpotLight(void);
		~SpotLight(void);

		void init(Eigen::Vector3f Position, Eigen::Vector3f Direction, Eigen::Vector3f Color, float Intensity, Eigen::Vector3f Attenuation, Eigen::Vector2f CutOff);
		
		void attenuation(Eigen::Vector3f Attenuation);
		void cutOff(Eigen::Vector2f CutOff);

		Eigen::Vector3f attenuation(void)const;
		Eigen::Vector2f cutOff(void)const;

	protected:
		Eigen::Vector3f m_Attenuation;
		Eigen::Vector2f m_CutOff;
	};//SpotLIght

}//name space

#endif 