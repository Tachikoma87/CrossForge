/*****************************************************************************\
*                                                                           *
* File(s): AnimationIO.h and AnimationIO:cpp                                 *
*                                                                           *
* Content: Stores and loads skeletal animation to/from files.   *
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
#ifndef __CFORGE_ANIMATIONIO_H__
#define __CFORGE_ANIMATIONIO_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"

namespace CForge {
	class AnimationIO {
	public:
		static void storeSkeletalAnimation(std::string Filepath, T3DMesh<float>* pMesh, uint32_t startIndex, uint32_t endIndex);
		static void loadSkeletalAnimation(std::string Filepath, T3DMesh<float>* pMesh);

	protected:
		AnimationIO(void);
		~AnimationIO(void);

	};//AnimationIO

}//namespace

#endif 