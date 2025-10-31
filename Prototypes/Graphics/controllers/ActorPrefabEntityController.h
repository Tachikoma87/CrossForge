/*****************************************************************************\
*                                                                           *
* File(s): ActorPrefabEntityController.h and ActorPrefabEntityController.cpp                       *
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
#ifndef __CROSSFORGE_ACTORPREFABENTITYCONTROLLER_H__
#define __CROSSFORGE_ACTORPREFABENTITYCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include "../entities/ActorPrefabEntity.h"
#include <crossforge/assetio/entities/TriangleMeshEntity.h>


namespace crossforge {
	class ActorPrefabEntityController : public ControllerBase {
	public:
		static inline std::string identification = "ActorPrefabEntityController";

		static bool buildStaticActor(ActorPrefabEntityPtr pActorEntity, TriangleMeshEntityPtr pTriangleMeshEntity);
		

		~ActorPrefabEntityController();
	protected:
		ActorPrefabEntityController();
		ActorPrefabEntityController(const std::string childIdentification);

		static bool buildVertexBuffer(ActorPrefabEntityPtr pActorEntity, TriangleMeshEntityPtr pMeshEntity);
		static bool buildIndexBuffer(ActorPrefabEntityPtr pActorEntity, TriangleMeshEntityPtr pTriangleMeshEntity);
		static bool buildRenderGroups(ActorPrefabEntityPtr pActorEntity, TriangleMeshEntityPtr pTriangleMeshEntity);
		static bool setVertexAttributePointer(ActorPrefabEntityPtr pActorEntity);
		static bool buildPbrMaterials(ActorPrefabEntityPtr pActorEntity, TriangleMeshEntityPtr pTriangleMeshEntity);

	};

	typedef std::shared_ptr<ActorPrefabEntityController> ActorEntityControllerPtr;

}

#endif 