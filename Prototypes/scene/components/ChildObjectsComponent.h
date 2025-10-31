/*****************************************************************************\
*                                                                           *
* File(s): ChildObjectsComponent.h and ChildObjectsComponent.cpp    *
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
#ifndef __CROSSFORGE_CHILDOBJECTSCOMPONENT_H__
#define __CROSSFORGE_CHILDOBJECTSCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>
#include "../entities/SceneObjectEntity.h"

namespace crossforge {
	class ChildObjectsComponent: public ComponentBase {
	public:
		static inline std::string identification = "ChildObjectsComponent";

		ChildObjectsComponent();
		~ChildObjectsComponent();

		void initialize();
		void clear();

		std::vector<SceneObjectEntityPtr> &childSceneObjects();

		bool addChild(SceneObjectEntityPtr pSceneObject);
		bool removeChild(SceneObjectEntityPtr pSceneObject);
		bool hasChild(SceneObjectEntityPtr pSceneObject);

	protected:
		ChildObjectsComponent(const std::string childIdentification);

		std::vector<SceneObjectEntityPtr> m_childSceneObjects;
	};

	typedef std::shared_ptr<ChildObjectsComponent> ChildObjectsComponentPtr;
}

#endif