/*****************************************************************************\
*                                                                           *
* File(s): CameraCapture.h and CameraCapture.cpp                *
*                                                                           *
* Content:                 *
*                                                                           *
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
#ifndef __CFORGE_CAMERACAPTURE_H__
#define __CFORGE_CAMERACAPTURE_H__
#include <crossforge/Core/CForgeObject.h>

#ifdef _WIN32
#include <Windows.h>

namespace CForge {
	class CameraCapture {
	public:
		CameraCapture();
		~CameraCapture();

		void init();
		void clear();
		void release();

	protected:

	};//CameraCapture
}//name-space

#endif


#endif 