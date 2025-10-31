/*****************************************************************************\
*                                                                           *
* File(s): GraphicsUtility.h and GraphicsUtility.cpp                        *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_GRAPHICSUTILITY_H__
#define __CROSSFORGE_GRAPHICSUTILITY_H__

#include <crossforge/core/CoreDefinitions.h>

namespace crossforge {
	class GraphicsUtility {
	public:

		/**
		* \brief Check whether an OpenGL error has occurred.
		*
		* \param[out] pVerbose Written explanation of the occurred error, if any. String is empty if no error occurred.
		* \return Error code equal to OpenGL error code. 0 if no error occurred.
		*/
		static uint32_t checkGLError(std::string* pVerbose);

	protected:
		GraphicsUtility();
		~GraphicsUtility();

	};

}

#endif 