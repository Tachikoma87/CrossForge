#include "TextureCoordinatesComponent.h"

namespace crossforge {

	TextureCoordinateComponent::TextureCoordinateComponent(): ComponentBase(TextureCoordinateComponent::identification) {

	}

	TextureCoordinateComponent::~TextureCoordinateComponent() {

	}

	void TextureCoordinateComponent::initialize() {
		clear();
	}

	void TextureCoordinateComponent::clear() {
		m_textureCoordinates.clear();
	}

	void TextureCoordinateComponent::setTextureCoordinates(std::vector<Eigen::Vector3f> textureCoordinates) {
		m_textureCoordinates = textureCoordinates;
	}

	void TextureCoordinateComponent::setTextureCoordinate(Eigen::Vector3f textureCoordinate, uint32_t index) {
		if (index >= m_textureCoordinates.size()) throw IndexOutOfBoundsExcept("index");
		m_textureCoordinates[index] = textureCoordinate;
	}

	std::vector<Eigen::Vector3f> TextureCoordinateComponent::getTextureCoordinates()const {
		return m_textureCoordinates;
	}

	std::vector<Eigen::Vector3f>& TextureCoordinateComponent::getTextureCoordinates() {
		return m_textureCoordinates;
	}

	Eigen::Vector3f TextureCoordinateComponent::getTextureCoordinate(uint32_t index)const {
		if (index >= m_textureCoordinates.size()) throw IndexOutOfBoundsExcept("index");
		return m_textureCoordinates[index];
	}

	Eigen::Vector3f& TextureCoordinateComponent::getTextureCordinate(uint32_t index) {
		if (index >= m_textureCoordinates.size()) throw IndexOutOfBoundsExcept("index");
		return m_textureCoordinates[index];
	}

	uint32_t TextureCoordinateComponent::getTextureCoorindatesCount()const {
		return m_textureCoordinates.size();
	}

}