#include "VideoEntity.h"

namespace crossforge {

	VideoEntity::VideoEntity(uint8_t componentsMap): EntityBase(VideoEntity::identification) {
		initialize(componentsMap);
	}
	VideoEntity::VideoEntity(const std::string childIdentification): EntityBase(VideoEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}
	VideoEntity::~VideoEntity() {
		clear();
	}

	void VideoEntity::initialize(uint8_t componentsMap) {
		clear();
		if (COMPONENT_VIDEO_DATA & componentsMap) this->addComponent(std::make_shared<VideoDataComponent>());
	}
	void VideoEntity::clear() {
		m_componentMap.clear();
	}

	VideoDataComponentPtr VideoEntity::getVideoDataComponent() {
		return getComponent<VideoDataComponent>();
	}


}