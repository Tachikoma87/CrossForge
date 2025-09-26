#include "MeshDefinitionsComponent.h"


namespace CForge {

	MeshDefinitionsComponent::MeshDefinitionsComponent(): ComponentBase(MeshDefinitionsComponent::identification) {

	}

	MeshDefinitionsComponent::~MeshDefinitionsComponent() {

	}

	void MeshDefinitionsComponent::initialize() {
		clear();
	}

	void MeshDefinitionsComponent::clear() {
		m_meshes.clear();
	}

	void MeshDefinitionsComponent::addMeshDefinition(MeshDefinitionPtr pMeshDefinition) {
		m_meshes.push_back(pMeshDefinition);
	}

	MeshDefinitionPtr MeshDefinitionsComponent::getMeshDefinition(uint32_t index) {
		if (index >= m_meshes.size()) throw IndexOutOfBoundsExcept("index");
		return m_meshes[index];
	}

	uint32_t MeshDefinitionsComponent::getMeshDefinitionsCount()const {
		return m_meshes.size();
	}

}