/*****************************************************************************\
*                                                                           *
* File(s): ActorInstanceEntityController.h and ActorInstanceEntityController.cpp                       *
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
#ifndef __CROSSFORGE_ACTORINSTANCEENTITYCONTROLLER_H__
#define __CROSSFORGE_ACTORINSTANCEENTITYCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include "../entities/ActorInstanceEntity.h"

namespace crossforge {
	class ActorInstanceEntityController : public ControllerBase {
		static inline std::string identification = "ActorInstanceEntityController";

	protected:
		ActorInstanceEntityController(const std::string childIdentification);
		~ActorInstanceEntityController();
	};

	using ActorInstanceEntityControllerPtr = std::shared_ptr<ActorInstanceEntityController>;
	using ActorInstanceEntityControllerCPtr = std::shared_ptr<const ActorInstanceEntityController>;
}

#endif 