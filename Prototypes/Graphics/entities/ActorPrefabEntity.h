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

		enum ActorPrefabEntityComponents {
			VERTEX_BUFFER_COMPONENT = 0x01,
			INDEX_BUFFER_COMPONENT = 0x02,
			RENDER_GROUPS_COMPONENT = 0x04,
			PBR_MATERIALS_COMPONENT = 0x08,
			VERTEX_ARRAY_COMPONENT = 0x10,
			COMPONENT_ACTOR_PREFAB_PROPERTIES = 0x20,
			COMPONENTS_ALL = 0xFF,
		};

		ActorPrefabEntity(uint8_t componentsBitmask = 0);
		~ActorPrefabEntity();

		void initialize(uint8_t componentsBitmask);
		void clear();

		VertexBufferComponentPtr getVertexBufferComponent();
		IndexBufferComponentPtr getIndexBufferComponent();
		RenderGroupsComponentPtr getRenderGroupsComponent();
		PbrMaterialsComponentPtr getPBRMaterialsComponent();
		VertexArrayComponentPtr getVertexArrayComponent();
		ActorPrefabPropertiesComponentPtr getActorPrefabPropertiesComponent();


	protected:
		ActorPrefabEntity(const std::string childIdentification);

	};

	typedef std::shared_ptr<ActorPrefabEntity> ActorPrefabEntityPtr;
}

#endif