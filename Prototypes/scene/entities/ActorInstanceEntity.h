/*****************************************************************************\
*                                                                           *
* File(s): ActorInstanceEntity.h and ActorInstanceEntity.cpp                *
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
#ifndef __CROSSFORGE_ACTORINSTANCEENTITY_H__
#define __CROSSFORGE_ACTORINSTANCEENTITY_H__

#include "SceneObjectEntity.h"
#include "../components/Transformation3DComponent.h"
#include <crossforge/graphics/components/ubos/UboTransformationDataComponent.h>

namespace crossforge {
	class ActorInstanceEntity : public SceneObjectEntity {
	public:
		static inline std::string identification = "ActorInstanceEntity";

		ActorInstanceEntity();
		~ActorInstanceEntity();

		void initialize();
		void clear();

		UboTransformationDataComponentPtr getUboTransformationDataComponent(const bool createIfNotExists = false);

	protected:
		ActorInstanceEntity(const std::string childIdentification);

	};
	using ActorInstanceEntityPtr = std::shared_ptr<ActorInstanceEntity>;
	using ActorInstanceEntityCPtr = std::shared_ptr<const ActorInstanceEntity>;
}

#endif 