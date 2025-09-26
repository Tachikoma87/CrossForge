#include "MaterialDataComponent.h"

namespace CForge {

	MaterialDataComponent::MaterialDataComponent(): ComponentBase(MaterialDataComponent::identification) {

	}

	MaterialDataComponent::~MaterialDataComponent() {

	}

	void MaterialDataComponent::initialize() {
		clear();
	}
	void MaterialDataComponent::clear() {
		m_materials.clear();
	}

	std::vector<MeshMaterialPtr>& MaterialDataComponent::getMaterials() {
		return m_materials;
	}
	std::vector<MeshMaterialPtr> MaterialDataComponent::getMaterials()const {
		return m_materials;
	}
	MeshMaterialPtr MaterialDataComponent::getMaterial(uint32_t index) {
		if (index >= m_materials.size()) throw IndexOutOfBoundsExcept("index");
		return m_materials[index];
	}
	void MaterialDataComponent::setMaterial(MeshMaterialPtr pMaterial, uint32_t index) {
		if (index >= m_materials.size()) throw IndexOutOfBoundsExcept("index");
		m_materials[index] = pMaterial;
	}

	void MaterialDataComponent::addMaterial(MeshMaterialPtr pMaterial) {
		m_materials.push_back(pMaterial);
	}
	void MaterialDataComponent::removeMaterial(uint32_t index) {
		if (index > m_materials.size()) throw IndexOutOfBoundsExcept("index");
		m_materials.erase(m_materials.begin() + index);
	}

	uint32_t MaterialDataComponent::getMaterialCount()const {
		return m_materials.size();
	}
}