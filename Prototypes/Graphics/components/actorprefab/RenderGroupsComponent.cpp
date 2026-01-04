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

	void RenderGroupsComponent::initialize(const std::shared_ptr<const RenderGroupsComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_renderGroups = pRef->getRenderGroups();
			m_renderGroupsMaterial = pRef->getRenderGroupsMaterial();
		}
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

	const std::vector<Eigen::Vector2i> RenderGroupsComponent::getRenderGroups()const {
		return m_renderGroups;
	}
	const std::vector<int32_t> RenderGroupsComponent::getRenderGroupsMaterial()const {
		return m_renderGroupsMaterial;
	}

	void RenderGroupsComponent::setRenderGroups(const std::vector<Eigen::Vector2i> renderGroups) {
		m_renderGroups = renderGroups;
	}
	void RenderGroupsComponent::setRenderGroupsMaterial(const std::vector<int32_t> renderGroupsMaterial) {
		m_renderGroupsMaterial = renderGroupsMaterial;
	}
}