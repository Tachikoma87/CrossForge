#include "RenderGroupsComponent.h"

namespace crossforge {

	RenderGroupsComponent::RenderGroupsComponent(): ComponentBase(RenderGroupsComponent::identification) {

	}
	RenderGroupsComponent::RenderGroupsComponent(const std::string childIdentification): ComponentBase(RenderGroupsComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}
	RenderGroupsComponent::~RenderGroupsComponent() {

	}

	std::vector<Eigen::Vector2i>& RenderGroupsComponent::renderGroups() {
		return m_renderGroups;
	}
	std::vector<int32_t>& RenderGroupsComponent::renderGroupsMaterial() {
		return m_renderGroupsMaterial;
	}
}