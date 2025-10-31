#include "TextureEntity.h"

namespace crossforge {

	TextureEntity::TextureEntity(uint8_t componentsMask): EntityBase(TextureEntity::identification) {
		initialize(componentsMask);
	}
	TextureEntity::TextureEntity(const std::string childIdentification): EntityBase(TextureEntity::identification) {
		m_inheritance.push_back(childIdentification);
		
	}
	TextureEntity::~TextureEntity() {
		clear();
	}

	void TextureEntity::initialize(uint8_t componentsMask) {
		clear();
		if (componentsMask & COMPONENT_TEXUTRE2D) addComponent(std::make_shared<Texture2DComponent>());
	}
	void TextureEntity::clear() {
		m_componentMap.clear();
	}

	Texture2DComponentPtr TextureEntity::getTexture2DComponent() {
		return getComponent<Texture2DComponent>();
	}

}