/*****************************************************************************\
*                                                                           *
* File(s): MorphTargetAnimationController.h and MorphTargetAnimationController.cpp                        *
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
#ifndef __CFORGE_MORPHTARGETANIMATIONCONTROLLER_H__
#define __CFORGE_MORPHTARGETANIMATIONCONTROLLER_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"
#include "../../CForge/Graphics/GLBuffer.h"

namespace CForge {
	class MorphTargetAnimationController {
	public:
		MorphTargetAnimationController(void);
		~MorphTargetAnimationController(void);

		void init(T3DMesh<float>* pMesh);
		void clear(void);

	protected:
		GLBuffer m_MorphTargetBuffer;
		uint32_t m_MTOffset; ///< Offset from one morph target data to next
	};//MorphTargetAnimationController
}//name space

#endif 