#include "RenderGroupsComponent.h"

namespace crossforge {

	RenderGroupsComponent::RenderGroupsComponent(): ComponentBase(RenderGroupsComponent::identification) {
		initialize();
	}
	RenderGroupsComponent::RenderGroupsComponent(const std::string childIdentification): ComponentBase(RenderGroupsComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}
	RenderGroupsComponent::~RenderGroupsComponent() {
		clear();
	}

	void RenderGroupsComponent::initialize() {
		clear();
	}
	void RenderGroupsComponent::clear() {
		m_renderGroups.clear();
		m_renderGroupsMaterial.clear();
	}

	std::vector<Eigen::Vector2i>& RenderGroupsComponent::renderGroups() {
		return m_renderGroups;
	}
	std::vector<int32_t>& RenderGroupsComponent::renderGroupsMaterial() {
		return m_renderGroupsMaterial;
	}
}