#include "WindowPropertiesComponent.h"

namespace CForge {

	WindowPropertiesComponent::WindowPropertiesComponent(): ComponentBase(WindowPropertiesComponent::identification) {
		initialize();
	}

	WindowPropertiesComponent::~WindowPropertiesComponent() {
		clear();
	}

	void WindowPropertiesComponent::initialize() {
		clear();
	}
	void WindowPropertiesComponent::clear() {
		m_width = 1280;
		m_height = 720;
		m_position = Eigen::Vector2i::Zero();
		m_title = "Basic CrossForge Window";

		m_glMinorVersion = 0;
		m_glMajorVersion = 0;
		m_pGlfwWindow = nullptr;
		m_multisampling = 0;
		m_vsync = 1;
	}

	uint32_t& WindowPropertiesComponent::width() {
		return m_width;
	}
	uint32_t& WindowPropertiesComponent::height() {
		return m_height;
	}
	std::string& WindowPropertiesComponent::title() {
		return m_title;
	}
	Eigen::Vector2i WindowPropertiesComponent::position() {
		return m_position;
	}

	uint32_t& WindowPropertiesComponent::glMinorVersion() {
		return m_glMinorVersion;
	}
	uint32_t& WindowPropertiesComponent::glMajorVersion() {
		return m_glMajorVersion;
	}
	uint32_t& WindowPropertiesComponent::multisampling() {
		return m_multisampling;
	}
	uint32_t WindowPropertiesComponent::vsync() {
		return m_vsync;
	}

	void* WindowPropertiesComponent::getGlfwWindow() {
		return m_pGlfwWindow;
	}
	void WindowPropertiesComponent::setGlfwWindow(void* pGlfwWindow) {
		m_pGlfwWindow = pGlfwWindow;
	}
}