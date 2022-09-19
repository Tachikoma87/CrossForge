/*****************************************************************************\
*                                                                           *
* File(s): Character.h and Character.cpp                                    *
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
#ifndef __CFORGE_CHARACTER_H__
#define __CFORGE_CHARACTER_H__

//<<<<<<<< HEAD:Prototypes/Camera/StripPhotoCamera.h

#include <opencv2/opencv.hpp>
#include <Eigen/Eigen>
#include "../../CForge/Core/CrossForgeException.h"
//#include "StripPhoto.h"
//========
#include "../Core/CForgeObject.h"
#include "../Core/ITCaller.hpp"
//>>>>>>>> dev_lod_decoration_acf:CForge/Input/Character.h

// TODO check if this is needed
namespace CForge {
	class CFORGE_API Character : public ITCaller<char32_t>, public CForgeObject {
	public:
		Character(void);
		~Character(void);

		void init(class GLFWwindow *pWin);
		void clear(void);

		void sendChar(unsigned int codepoint);
	protected:
		class GLFWwindow* m_pWin;
	};//Character

}//name-space

#endif
