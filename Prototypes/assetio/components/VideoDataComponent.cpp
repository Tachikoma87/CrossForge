#include "VideoDataComponent.h"

namespace crossforge {

	VideoDataComponent::VideoDataComponent(): ComponentBase(VideoDataComponent::identification) {
		initialize();
	}

	VideoDataComponent::~VideoDataComponent() {
		clear();
	}
	VideoDataComponent::VideoDataComponent(const std::string childIdentification):ComponentBase(VideoDataComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void VideoDataComponent::initialize() {
		clear();
	}
	void VideoDataComponent::clear() {
		m_framerate = 0.0f;
		m_width = 0;
		m_height = 0;
		m_filename.clear();
		m_isRecording = false;
	}

	float& VideoDataComponent::framerate() {
		return m_framerate;
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

	std::shared_ptr<void>& VideoDataComponent::videoData() {
		return m_pVideoData;
	}
	std::shared_ptr<void>& VideoDataComponent::outputStream() {
		return m_pOutputStream;
	}
}