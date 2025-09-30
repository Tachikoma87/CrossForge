#include "MeshDefinition.h"

namespace crossforge {

	MeshDefinition::MeshDefinition() {

	}

	MeshDefinition::~MeshDefinition() {
		clear();
	}

	void MeshDefinition::initialize() {
		clear();
	}
	void MeshDefinition::clear() {
		m_faces.clear();
	}

	Eigen::Vector3i MeshDefinition::operator[](uint32_t index)const {
		return getFace(index);
	}

	Eigen::Vector3i& MeshDefinition::operator[](uint32_t index) {
		return getFace(index);
	}

	void MeshDefinition::setFaces(std::vector<Eigen::Vector3i> faces) {
		m_faces = faces;
	}

	void MeshDefinition::setFace(Eigen::Vector3i face, uint32_t index) {
		if (index >= getFaceCount()) throw IndexOutOfBoundsExcept("index");
		m_faces[index] = face;
	}

	std::vector<Eigen::Vector3i>& MeshDefinition::getFaces() {
		return m_faces;
	}
	std::vector<Eigen::Vector3i> MeshDefinition::getFaces()const {
		return m_faces;
	}
	Eigen::Vector3i MeshDefinition::getFace(uint32_t index)const {
		if (index >= getFaceCount()) throw IndexOutOfBoundsExcept("index");
		return m_faces[index];
	}
	Eigen::Vector3i& MeshDefinition::getFace(uint32_t index) {
		if (index >= getFaceCount()) throw IndexOutOfBoundsExcept("index");
		return m_faces[index];
	}

	uint32_t MeshDefinition::getFaceCount()const {
		return m_faces.size();
	}

	void MeshDefinition::setMaterialIndex(int32_t index) {
		m_materialIndex = index;
	}

	int32_t MeshDefinition::getMaterialindex(void)const {
		return m_materialIndex;
	}
}