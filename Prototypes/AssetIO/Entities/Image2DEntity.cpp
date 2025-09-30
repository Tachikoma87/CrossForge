#include "Image2DEntity.h"

namespace crossforge {
	Image2DEntity::Image2DEntity(uint8_t componentBitmask): EntityBase(Image2DEntity::identification) {
		m_inheritance.push_back(Image2DEntity::identification);
		initialize(componentBitmask);
	}
	Image2DEntity::~Image2DEntity() {
		clear();
	}

	void Image2DEntity::initialize(uint8_t componentBitmask) {
		clear();
		if (componentBitmask & RAW_IMAGE_2D_DATA_COMPONENT) addComponent(std::make_shared<RawImage2DDataComponent>());
	}
	void Image2DEntity::clear() {
		m_componentMap.clear();
	}

	RawImage2DDataComponentptr Image2DEntity::getRawImage2DDataComponent() {
		return getComponent<RawImage2DDataComponent>();
	}
}