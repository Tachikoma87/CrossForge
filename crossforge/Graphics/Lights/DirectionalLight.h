/*****************************************************************************\
*                                                                           *
* File(s): DirectionalLight.h and DirectionalLight.cpp                                *
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
#ifndef __CFORGE_DIRECTIONALLIGHT_H__
#define __CFORGE_DIRECTIONALLIGHT_H__

#include "ILight.h"


namespace CForge {

	/**
	* \brief A simple directional light. Has only a direction and position for shadow casting.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API DirectionalLight : public ILight {
	public:
		DirectionalLight(void);
		~DirectionalLight(void);

	protected:

	};//SunLight

}//name space

#endif 