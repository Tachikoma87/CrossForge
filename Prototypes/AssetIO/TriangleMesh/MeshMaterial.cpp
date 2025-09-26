#include "MeshMaterial.h"

namespace CForge {

	MeshMaterial::MeshMaterial() {
		initialize();
	}

	MeshMaterial::~MeshMaterial() {
		clear();
	}

	void MeshMaterial::initialize() {
		clear();
	}
	void MeshMaterial::clear() {
		for (uint8_t i = 0; i < TEXTURE_TYPE_COUNT; ++i) m_texturePaths[i] = "";
		for (uint8_t i = 0; i < COLOR_TYPE_COUNT; ++i) m_colors[i] = Eigen::Vector4f::Zero();
		m_metallic = 0.0f;
		m_roughness = 0.0f;
		m_shininess = 0.0f;
	}

	std::string MeshMaterial::getTexturePath(TextureType type)const {
		if (type >= TEXTURE_TYPE_COUNT) throw IndexOutOfBoundsExcept("type");
		return m_texturePaths[type];
	}
	std::string MeshMaterial::getTexturePath(TextureType type) {
		if (type >= TEXTURE_TYPE_COUNT) throw IndexOutOfBoundsExcept("type");
		return m_texturePaths[type];
	}
	Eigen::Vector4f MeshMaterial::getColor(ColorType type)const {
		if (type >= COLOR_TYPE_COUNT) throw IndexOutOfBoundsExcept("type");
		return m_colors[type];
	}
	Eigen::Vector4f MeshMaterial::getColor(ColorType type) {
		if (type >= COLOR_TYPE_COUNT) throw IndexOutOfBoundsExcept("type");
		return m_colors[type];
	}
	float MeshMaterial::getMetallic()const {
		return m_metallic;
	}
	float MeshMaterial::getRoughness()const {
		return m_roughness;
	}
	float MeshMaterial::getShininess()const {
		return m_shininess;
	}

	void MeshMaterial::setTexturePath(std::string texturePath, TextureType type) {
		if (type >= TEXTURE_TYPE_COUNT) throw IndexOutOfBoundsExcept("type");
		m_texturePaths[type] = texturePath;
	}
	void MeshMaterial::setColor(Eigen::Vector4f color, ColorType type) {
		if (type >= COLOR_TYPE_COUNT) throw IndexOutOfBoundsExcept("type");
		m_colors[type] = color;
	}
	void MeshMaterial::setMetallica(float metallic) {
		m_metallic = metallic;
	}
	void MeshMaterial::setRoughness(float roughness) {
		m_roughness = roughness;
	}
	void MeshMaterial::setShininess(float shininess) {
		m_shininess = shininess;
	}

}