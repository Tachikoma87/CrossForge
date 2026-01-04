#include "TextureEntity.h"

namespace crossforge {

	TextureEntity::TextureEntity(): EntityBase(TextureEntity::identification) {
		initialize();
	}
	TextureEntity::TextureEntity(const std::string childIdentification): EntityBase(TextureEntity::identification) {
		m_inheritance.push_back(childIdentification);
		
	}
	TextureEntity::~TextureEntity() {
		clear();
	}

	void TextureEntity::initialize() {
		clear();
	}
	void TextureEntity::clear() {
		m_componentMap.clear();
	}

	Texture2DComponentPtr TextureEntity::getTexture2DComponent(bool createIfNotExists) {
		return getComponent<Texture2DComponent>(createIfNotExists);
	}

}