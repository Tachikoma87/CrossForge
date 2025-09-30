#include "BinaryDataComponent.h"

namespace crossforge {

	BinaryDataComponent::BinaryDataComponent() : ComponentBase(BinaryDataComponent::identification) {
		m_inheritance.push_back(BinaryDataComponent::identification);
	}
	BinaryDataComponent::~BinaryDataComponent() {
		clear();
	}

	void BinaryDataComponent::initialize() {
		clear();
	}
	void BinaryDataComponent::clear() {
		m_binaryData.clear();
	}

	std::vector<uint8_t>& BinaryDataComponent::binaryData() {
		return m_binaryData;
	}

	uint64_t BinaryDataComponent::getSize()const {
		return m_binaryData.size();
	}
}