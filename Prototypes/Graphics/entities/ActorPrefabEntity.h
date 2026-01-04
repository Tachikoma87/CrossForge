/*****************************************************************************\
*                                                                           *
* File(s): ActorPrefabEntity.h and ActorPrefabEntity.cpp                        *
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
#ifndef __CROSSFORGE_ACTORPREFABENTITY_H__
#define __CROSSFORGE_ACTORPREFABENTITY_H__

#include <crossforge/ecs/EntityBase.h>
#include "../components/actorprefab/VertexBufferComponent.h"
#include "../components/actorprefab/IndexBufferComponent.h"
#include "../components/actorprefab/RenderGroupsComponent.h"
#include "../components/actorprefab/PBRMaterialsComponent.h"
#include "../components/actorprefab/VertexArrayComponent.h"
#include "../components/actorprefab/ActorPrefabPropertiesComponent.h"

namespace crossforge {
	class ActorPrefabEntity : public EntityBase {
	public:
		inline static std::string identification = "ActorPrefabEntity";

		ActorPrefabEntity();
		~ActorPrefabEntity();

		void initialize();
		void clear();

		VertexBufferComponentPtr getVertexBufferComponent(bool createIfNotExists = false);
		IndexBufferComponentPtr getIndexBufferComponent(bool createIfNotExists = false);
		RenderGroupsComponentPtr getRenderGroupsComponent(bool createIfNotExists = false);
		PbrMaterialsComponentPtr getPBRMaterialsComponent(bool createIfNotExists = false);
		VertexArrayComponentPtr getVertexArrayComponent(bool createIfNotExists = false);
		ActorPrefabPropertiesComponentPtr getActorPrefabPropertiesComponent(bool createIfNotExists = false);


	protected:
		ActorPrefabEntity(const std::string childIdentification);

	};

	using ActorPrefabEntityPtr = std::shared_ptr<ActorPrefabEntity>;
	using ActorPrefabEntityCPtr = std::shared_ptr<const ActorPrefabEntity>;
}

#endif