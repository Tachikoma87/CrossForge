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

	void ColorComponent::initialize(const std::shared_ptr<const ColorComponent> pRef) {
		if (this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_color = pRef->getColor();
		}
	}
	void ColorComponent::clear() {
		m_color = Eigen::Vector4f::Ones();
	}

	Eigen::Vector4f& ColorComponent::color() {
		return m_color;
	}

	const Eigen::Vector4f ColorComponent::getColor()const {
		return m_color;
	}
	void ColorComponent::setColor(const Eigen::Vector4f color) {
		m_color = color;
	}


}