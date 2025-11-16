#include "ColorComponent.h"

namespace crossforge {

	ColorComponent::ColorComponent() : ComponentBase(ColorComponent::identification) {
		initialize();
	}
	ColorComponent::ColorComponent(const std::string childIdentification): ComponentBase(ColorComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
		
	}
	ColorComponent::~ColorComponent() {
		clear();
	}

	void ColorComponent::initialize() {
		clear();
	}
	void ColorComponent::clear() {
		m_color = Eigen::Vector4f::Ones();
	}

	Eigen::Vector4f& ColorComponent::color() {
		return m_color;
	}


}