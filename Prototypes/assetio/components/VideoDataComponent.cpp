#include "VideoDataComponent.h"

namespace crossforge {

	VideoDataComponent::VideoDataComponent(): ComponentBase(VideoDataComponent::identification) {

	}

	VideoDataComponent::~VideoDataComponent() {

	}
	VideoDataComponent::VideoDataComponent(const std::string childIdentification):ComponentBase(VideoDataComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}

	float& VideoDataComponent::framerate() {
		return m_framefrate;
	}
	uint32_t& VideoDataComponent::width() {
		return m_width;
	}
	uint32_t& VideoDataComponent::height() {
		return m_height;
	}
	std::string& VideoDataComponent::filename() {
		return m_filename;
	}
	bool& VideoDataComponent::isRecording() {
		return m_isRecording;
	}
}