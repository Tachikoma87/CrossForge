/*****************************************************************************\
*                                                                           *
* File(s): VideoController.h and VideoController.cpp                                               *
*                                                                           *
* Content:          *
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
#ifndef __CROSSFORGE_VIDEOCONTROLLER_H__
#define __CROSSFORGE_VIDEOCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>

namespace crossforge {
	class VideoController : public ControllerBase {
	public:
		static inline std::string identification = "VideoController";

	protected:
		VideoController(const std::string identification);
		~VideoController();
	};

	typedef std::shared_ptr<VideoController> VideoControllerPtr;
}

#endif 