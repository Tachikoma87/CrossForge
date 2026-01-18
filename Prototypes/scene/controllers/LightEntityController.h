/*****************************************************************************\
*                                                                           *
* File(s): LightEntityController.h and LightEntityController.cpp                       *
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
#ifndef __CROSSFORGE_LIGHTENTITYCONTROLLER_H__
#define __CROSSFORGE_LIGHTENTITYCONTROLLER_H__

#include <crossforge/eccs/ControllerBase.h>

namespace crossforge {
	class LightEntityController : public ControllerBase {
	public:
		static inline std::string identification = "LightEntityController";

		~LightEntityController();
	protected:
		LightEntityController();
		LightEntityController(const std::string childIdentification);
	};

	using LightEntityControllerPtr = std::shared_ptr<LightEntityController>;
	using LightEntityControllerCPtr = std::shared_ptr<const LightEntityController>;

}

#endif 