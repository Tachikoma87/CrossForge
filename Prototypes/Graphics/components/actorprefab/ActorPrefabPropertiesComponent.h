/*****************************************************************************\
*                                                                           *
* File(s): ActorPrefabPropertiesComponent.h and ActorPrefabPropertiesComponent.cpp                      *
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
#ifndef __CROSSFORGE_ACTORPREFABPROPERTIESCOMPONENT_H__
#define __CORSSFORGE_ACTORPREFABPROPERTIESCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class ActorPrefabPropertiesComponent : public ComponentBase {
	public:
		static inline std::string identification = "ActorPrefabPropertiesComponent";
		
		ActorPrefabPropertiesComponent();
		~ActorPrefabPropertiesComponent();

		void initialize(std::shared_ptr<ActorPrefabPropertiesComponent> pRef = nullptr);
		void clear() override;

		bool& propertyNormalMapping();
		bool& propertySkeltalAnimation();
		bool& propertyMorphTargetAnimation();

	protected:
		ActorPrefabPropertiesComponent(const std::string childIdentification);

		bool m_propertyNormalMapping;
		bool m_propertySkeletalAnimation;
		bool m_propertyMorphTargetAnimation;
	};

	typedef std::shared_ptr<ActorPrefabPropertiesComponent> ActorPrefabPropertiesComponentPtr;
}

#endif 