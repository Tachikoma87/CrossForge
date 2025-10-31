/*****************************************************************************\
*                                                                           *
* File(s): RenderGroupsComponent.h and RenderGroupsComponent.cpp                       *
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
#ifndef __CROSSFORGE_RENDERGROUPSCOMPONENT_H__
#define __CROSSFORGE_RENDERGROUPSCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class RenderGroupsComponent : public ComponentBase {
	public:
		static inline std::string identification = "RenderGroupsComponent";

		RenderGroupsComponent();
		~RenderGroupsComponent();

		std::vector<Eigen::Vector2i> &renderGroups(); 
		std::vector<int32_t> &renderGroupsMaterial();

	protected:
		RenderGroupsComponent(const std::string childIdentification);

		std::vector<Eigen::Vector2i> m_renderGroups;
		std::vector<int32_t> m_renderGroupsMaterial;
	};

	typedef std::shared_ptr<RenderGroupsComponent> RenderGroupsComponentPtr;
}

#endif 