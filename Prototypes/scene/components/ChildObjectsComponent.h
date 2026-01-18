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

#include <crossforge/eccs/ComponentBase.h>
#include "../entities/SceneObjectEntity.h"

namespace crossforge {
	class ChildObjectsComponent: public ComponentBase {
	public:
		static inline std::string identification = "ChildObjectsComponent";

		ChildObjectsComponent();
		~ChildObjectsComponent();

		void initialize(const std::shared_ptr<const ChildObjectsComponent> pRef = nullptr);
		void clear() override;

		bool addChild(SceneObjectEntityPtr pSceneObject);
		bool removeChild(SceneObjectEntityPtr pSceneObject);
		bool hasChild(SceneObjectEntityCPtr pSceneObject);

		std::vector<SceneObjectEntityPtr>& childSceneObjects();
		const std::vector<SceneObjectEntityCPtr> getChildSceneObjects()const;
		void setChildSceneObjects(const std::vector<SceneObjectEntityPtr> objects);

	protected:
		ChildObjectsComponent(const std::string childIdentification);

		std::vector<SceneObjectEntityPtr> m_childSceneObjects;
	};

	using ChildObjectsComponentPtr = std::shared_ptr<ChildObjectsComponent>;
	using ChildObjectsComponentCPtr = std::shared_ptr<const ChildObjectsComponent>;
}

#endif