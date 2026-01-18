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


#include <crossforge/eccs/ComponentBase.h>
#include <crossforge/scene/entities/SceneNodeEntity.h>

namespace crossforge {
	class TargetSceneNodeComponent : public ComponentBase {
	public:
		static inline std::string identification = "TargetObjectComponent";

		TargetSceneNodeComponent();
		~TargetSceneNodeComponent();

		void initialize(const std::shared_ptr<const TargetSceneNodeComponent> pRef = nullptr);
		void clear() override;

		SceneNodeEntityPtr& targetSceneNode();
		const SceneNodeEntityCPtr getTargetSceneNode()const;
		void setTargetSceneNode(const SceneNodeEntityPtr pNode);

	protected:
		TargetSceneNodeComponent(const std::string childIdentification);

		SceneNodeEntityPtr m_pTargetEntity;
	};
	using TargetSceneNodeComponentPtr = std::shared_ptr<TargetSceneNodeComponent>;
	using TargetSceneNodeComponentCPtr = std::shared_ptr<const TargetSceneNodeComponent>;
}

#endif 