/*****************************************************************************\
*                                                                           *
* File(s): ShaderEntityController.h and ShaderEntityController.cpp                        *
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
#ifndef __CROSSFORGE_SHADERENTITYCONTROLLER_H__
#define __CROSSFORGE_SHADERENTITYCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include "../entities/ShaderEntity.h"

namespace crossforge {
	class ShaderEntityController : public ControllerBase {
	public:
		static inline std::string identification = "ShaderEntityController";

		
		~ShaderEntityController();

	protected:
		ShaderEntityController();
		ShaderEntityController(const std::string childIdentification);


	};
	typedef std::shared_ptr<ShaderEntityController> ShaderEntityControllerPtr;
}

#endif 