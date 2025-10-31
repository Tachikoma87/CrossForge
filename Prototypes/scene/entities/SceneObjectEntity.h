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

		Transformation3DComponentPtr getTransformation3DComponent();
		Movement3DComponentPtr getMovement3DComponent();
		std::shared_ptr<class ChildObjectsComponent> getChildObjectsComponent();

	protected:
		SceneObjectEntity(const std::string childIdentification);

	};

	typedef std::shared_ptr<SceneObjectEntity> SceneObjectEntityPtr;
}



#endif 