/*****************************************************************************\
*                                                                           *
* File(s): TargetObjectComponent.h and TargetObjectComponent.cpp                *
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
#ifndef __CROSSFORGE_TARGETOBJECTCOMPONENT_H__
#define __CROSSFORGE_TARGETOBJECTCOMPONENT_H__

#include "../entities/SceneObjectEntity.h"
#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class TargetObjectComponent : public ComponentBase {
	public:
		static inline std::string identification = "TargetObjectComponent";

		TargetObjectComponent();
		~TargetObjectComponent();

		void initialize(const std::shared_ptr<const TargetObjectComponent> pRef = nullptr);
		void clear() override;

		SceneObjectEntityPtr& targetSceneObject();
		const SceneObjectEntityCPtr getTargetSceneObject()const;
		void setTargetSceneObject(const SceneObjectEntityPtr pObj);

	protected:
		TargetObjectComponent(const std::string childIdentification);

		SceneObjectEntityPtr m_pTargetEntity;
	};
	using TargetObjectComponentPtr = std::shared_ptr<TargetObjectComponent>;
	using TargetObjectComponentCPtr = std::shared_ptr<const TargetObjectComponent>;
}

#endif 