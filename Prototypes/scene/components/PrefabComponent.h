/*****************************************************************************\
*                                                                           *
* File(s): PrefabComponent.h and PrefabComponent.cpp               *
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
#ifndef __CROSSFORGE_PREFABCOMPONENT_H__
#define __CRSOFFROGE_PREFABCOMPONENT_H__

#include <crossforge/eccs/ComponentBase.h>
#include "../../Graphics/entities/ActorPrefabEntity.h"

namespace crossforge {
	class PrefabComponent : public ComponentBase {
	public:
		static inline std::string identification = "PrefabComponent";
		
		PrefabComponent();
		~PrefabComponent();

		void initialize(const std::shared_ptr<const PrefabComponent> pRef = nullptr);
		void clear() override;

		ActorPrefabEntityPtr& actorPrefab();
		ActorPrefabEntityCPtr getActorPrefab()const;
		void setActorPrefab(ActorPrefabEntityPtr pActorPrefab);

	protected:
		PrefabComponent(const std::string childIdentification);

		ActorPrefabEntityPtr m_pActorPrefab;

	};
	using PrefabComponentPtr = std::shared_ptr<PrefabComponent>;
	using PrefabComponentCPtr = std::shared_ptr<const PrefabComponent>;
}


#endif 