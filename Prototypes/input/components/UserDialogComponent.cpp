#include "UserDialogComponent.h"

namespace crossforge {

	UserDialogComponent::UserDialogComponent(): ComponentBase(UserDialogComponent::identification) {
		initialize();
	}
	UserDialogComponent::~UserDialogComponent() {

	}
	UserDialogComponent::UserDialogComponent(const std::string childIdentification): ComponentBase(UserDialogComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void UserDialogComponent::initialize() {
		clear();
	}
	void UserDialogComponent::clear() {
		m_dialogType = DIALOG_TYPE_UNKNOWN;
		m_iconType = ICON_TYPE_UNKNOWN;
		m_allowMultiselect = false;
	}

	std::string& UserDialogComponent::title() {
		return m_title;
	}
	std::string& UserDialogComponent::message() {
		return m_message;
	}

	UserDialogComponent::DialogType& UserDialogComponent::dialogType() {
		return m_dialogType;
	}
	UserDialogComponent::IconType& UserDialogComponent::iconType() {
		return m_iconType;
	}

	UserDialogComponent::MessageBoxType& UserDialogComponent::messageBoxType() {
		return m_messageBoxType;
	}

	std::string& UserDialogComponent::defaultInput() {
		return m_defaultInput;
	}

	std::string& UserDialogComponent::defaultPath() {
		return m_defaultPath;
	}
	std::vector<std::string>& UserDialogComponent::fileFilterPatterns() {
		return m_fileFilterPatterns;
	}

	bool& UserDialogComponent::allowMultiselect() {
		return m_allowMultiselect;
	}

}