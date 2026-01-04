/*****************************************************************************\
*                                                                           *
* File(s): MiscellaneousController.h and MiscellaneousController.cpp        *
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
#ifndef __CROSSFORGE_MISCELLANEOUSCONTROLLER_H__
#define __CROSSFORGE_MISCELLANEOUSCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>

namespace crossforge {
	class MiscellaneousController : public ControllerBase {
	public:
		static inline std::string identification = "MiscellaneousController";

		~MiscellaneousController();
	protected:
		MiscellaneousController(const std::string childIdentification);
	};

	typedef std::shared_ptr<MiscellaneousController> MiscellaneousControllerPtr;
}

#endif 