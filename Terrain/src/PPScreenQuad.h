/*****************************************************************************\
*                                                                           *
* File(s): ScreenQuad.h and ScreenQuad.cpp                                *
*                                                                           *
* Content:    *
*          .                                         *
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

#include "../../CForge/Graphics/Actors/ScreenQuad.h"

namespace CForge {
	/**
	* \brief A simple quad that covers a 2D area and can be rendered. Used for post processing, GBuffer lighting stage, and on screen drawing
	*
	* \todo Do full documentation
	*/
	class CFORGE_API PPScreenQuad : public ScreenQuad {
	public:
		void render(RenderDevice* pRDev, float mapHeight, float uTime, float aspectRatio);
	};//ScreenQuad
}//name space
