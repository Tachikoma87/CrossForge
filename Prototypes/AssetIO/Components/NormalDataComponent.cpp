#include "NormalDataComponent.h"

namespace CForge {
	NormalDataComponent::NormalDataComponent(): ComponentBase(NormalDataComponent::identification) {

	}

	NormalDataComponent::~NormalDataComponent() {
		clear();
	}

	void NormalDataComponent::initialize(std::vector<Eigen::Vector3f> normals) {
		m_normals = normals;
	}
	void NormalDataComponent::clear() {
		m_normals.clear();
	}

	const Eigen::Vector3f NormalDataComponent::operator[](const uint32_t index)const {
		Eigen::Vector3f result = Eigen::Vector3f::Zero();
		if (index >= getNormalCount()) {
			LogError("Index " + std::to_string(index) + " is out of bounds!", "");
		}
		else {
			result = m_normals[index];
		}
		return result;
	}

	Eigen::Vector3f& NormalDataComponent::operator[](const uint32_t index) {
		if (index >= getNormalCount()) throw IndexOutOfBoundsExcept("index");
		return m_normals[index];
	}

	void NormalDataComponent::setNormals(std::vector<Eigen::Vector3f> normals) {
		m_normals = normals;
	}

	void NormalDataComponent::setNormal(Eigen::Vector3f normal, uint32_t index) {
		(*this)[index] = normal;
	}

	std::vector<Eigen::Vector3f> NormalDataComponent::getNormals()const {
		return m_normals;
	}
	std::vector<Eigen::Vector3f>& NormalDataComponent::getNormals() {
		return m_normals;
	}

	Eigen::Vector3f NormalDataComponent::getNormal(uint32_t index)const {
		return (*this)[index];
	}

	Eigen::Vector3f& NormalDataComponent::getNormal(uint32_t index) {
		return (*this)[index];
	}
	uint32_t NormalDataComponent::getNormalCount()const {
		return m_normals.size();
	}
}