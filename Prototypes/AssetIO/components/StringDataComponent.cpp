#include "StringDataComponent.h"

namespace crossforge {

	StringDataComponent::StringDataComponent(): ComponentBase(StringDataComponent::identification) {
		m_inheritance.push_back(StringDataComponent::identification);
	}
	StringDataComponent::~StringDataComponent() {
		clear();
	}

	void StringDataComponent::initialize() {
		clear();
	}
	void StringDataComponent::clear() {
		m_stringData = "";
	}

	std::string& StringDataComponent::stringData() {
		return m_stringData;
	}
	uint64_t StringDataComponent::length()const {
		return m_stringData.length();
	}

}