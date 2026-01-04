/*****************************************************************************\
*                                                                           *
* File(s): SceneObjectEntity.h and SceneObjectEntity.cpp                        *
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
#ifndef __CROSSFORGE_SCENEOBJECTENTITY_H__
#define __CROSSFORGE_SCENEOBJECTENTITY_H__

#include <crossforge/ecs/EntityBase.h>
#include "../components/Transformation3DComponent.h"
#include "../components/Movement3DComponent.h"


namespace crossforge {
	class SceneObjectEntity : public EntityBase {
	public:
		static inline std::string identification = "SceneObjectEntity";

		SceneObjectEntity();
		~SceneObjectEntity();

		Transformation3DComponentPtr getTransformation3DComponent(const bool createIfNotExists = true);
		Movement3DComponentPtr getMovement3DComponent(const bool createIfNotExists = true);
		std::shared_ptr<class ChildObjectsComponent> getChildObjectsComponent(const bool createIfNotExists = true);

	protected:
		SceneObjectEntity(const std::string childIdentification);

	};

	using SceneObjectEntityPtr = std::shared_ptr<SceneObjectEntity>;
	using SceneObjectEntityCPtr = std::shared_ptr<const SceneObjectEntity>;
}



#endif 