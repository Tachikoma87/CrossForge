#include "ColorComponent.h"

namespace crossforge {

	ColorComponent::ColorComponent() : ComponentBase(ColorComponent::identification) {
		m_color = Eigen::Vector4f::Ones();
	}
	ColorComponent::ColorComponent(const std::string childIdentification): ComponentBase(ColorComponent::identification) {
		m_inheritance.push_back(childIdentification);
		m_color = Eigen::Vector4f::Ones();
	}
	ColorComponent::~ColorComponent() {

	}

	Eigen::Vector4f& ColorComponent::color() {
		return m_color;
	}


}