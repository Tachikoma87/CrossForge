#include "UserInputComponent.h"

namespace crossforge {

	UserInputComponent::UserInputComponent(): ComponentBase(UserInputComponent::identification) {
		initialize();
	}
	UserInputComponent::~UserInputComponent() {

	}
	UserInputComponent::UserInputComponent(const std::string childIdentification): ComponentBase(UserInputComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void UserInputComponent::initialize() {
		clear();
	}
	void UserInputComponent::clear() {
		m_rgbColor = Eigen::Vector3f::Zero();
		m_string = "";
	}

	Eigen::Vector3f& UserInputComponent::rgbColor() {
		return m_rgbColor;
	}
	std::string& UserInputComponent::string() {
		return m_string;
	}

	UserInputComponent::UserAnswer& UserInputComponent::userAnswer() {
		return m_userAnswer;
	}

	std::vector<std::string>& UserInputComponent::fileList() {
		return m_fileList;
	}

	std::string& UserInputComponent::directory() {
		return m_directory;
	}

}