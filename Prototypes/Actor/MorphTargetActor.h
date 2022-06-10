/*****************************************************************************\
*                                                                           *
* File(s): MorphTargetActor.h and MorphTargetActor.cpp                          *
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
#ifndef __CFORGE_MORPHTARGETACTOR_H__
#define __CFROGE_MORPHTARGETACTOR_H__

#include "../../CForge/Graphics/Actors/IRenderableActor.h"

namespace CForge {
	class MorphTargetActor : public IRenderableActor {
	public:
		void init(T3DMesh<float>* pMesh);

		void render(class RenderDevice* pRDev);

	protected:

	};//MorphTargetActor
}//name-space

#endif 