#include "BasicWindowSetup.h"

#include <crossforge/graphics/controllers/WindowEntityController.h>
#include <crossforge/math/CrossForgeMath.h>

namespace crossforge {

	BasicWindowSetup::BasicWindowSetup(): ExampleBase("CrossForge Example - Simple Graphics Setup") {
		m_frameCounter = 0;
		m_lastFpsPrint = 0;
		m_frameProcessingCounter = 0;

		
		m_bgColors.push_back(Eigen::Vector4f(239, 222, 205, 255)); // Almond
		m_bgColors.push_back(Eigen::Vector4f(210, 217, 219, 255)); // Aluminum Foil
		m_bgColors.push_back(Eigen::Vector4f(159, 43, 104, 255)); // Amaranth deep purple
		m_bgColors.push_back(Eigen::Vector4f(52, 179, 52, 255)); // American green
		m_bgColors.push_back(Eigen::Vector4f(29, 172, 214, 255)); // Battery charged blue
		m_bgColors.push_back(Eigen::Vector4f(37, 35, 33, 255)); // Black checstnut oak
		m_bgColors.push_back(Eigen::Vector4f(205, 127, 50, 255)); // Bronze

		for (auto& color : m_bgColors) color /= 255.0f;
	}

	BasicWindowSetup::~BasicWindowSetup() {

	}

	void BasicWindowSetup::initialize() {

		// create window
		m_pWindowSystem = std::make_shared<WindowSystem>();
		m_pWindowSystem->initialize();
		
		m_pMainWindow = std::make_shared<WindowEntity>();
		auto pWindowPropComponent = m_pMainWindow->getWindowPropertiesComponent(true);
		pWindowPropComponent->clearColor() = Eigen::Vector4f(0.2f, 0.2f, 0.2f, 1.0f);
		pWindowPropComponent->width() = 1280;
		pWindowPropComponent->height() = 720;
		pWindowPropComponent->position() = Eigen::Vector2i(200, 200);
		pWindowPropComponent->title() = m_exampleName;
		if (WindowEntityController::initOpenGLWindow(m_pMainWindow)) LogInfo("Created CrossForge - simple example window.");
		else LogError("Failed to create main window.");
		m_pWindowSystem->registerEntity(m_pMainWindow);

		// create a canvas to draw on


		// create mouse and keyboard input
		m_pInputDevice = std::make_shared<InputDeviceEntity>();
		m_pInputDevice->getAssociatedWindowComponent(true)->associatedWindow() = m_pMainWindow;
	
		auto pMouseInputSystem = std::make_shared<MouseInputSystem>();
		pMouseInputSystem->initialize();
		pMouseInputSystem->registerEntity(m_pInputDevice);
		SystemManager::instance()->addSystem(pMouseInputSystem);

		auto pKeyboardInputSystem = std::make_shared<KeyboardInputSystem>();
		pKeyboardInputSystem->initialize();
		pKeyboardInputSystem->registerEntity(m_pInputDevice);
		SystemManager::instance()->addSystem(pKeyboardInputSystem);
	
	}

	void BasicWindowSetup::update() {
		uint64_t timestampFrameStart = GeneralUtility::timestampHighPrecision();

		m_pWindowSystem->update();
		WindowEntityController::clearBuffer(m_pMainWindow);


		if (nullptr == m_pMainWindow->getWindowPropertiesComponent()->getGlfwWindowHandle()) this->stop();
		if (KeyboardStateComponent::KEY_STATE_PRESSED == m_pInputDevice->getKeyboardStateComponent()->getKeyState(KeyboardStateComponent::KEY_ESCAPE)) this->stop();
		if (MouseStateComponent::BUTTON_STATE_PRESSED == m_pInputDevice->getMouseStateComponent()->getButtonState(MouseStateComponent::BUTTON_LEFT)) {
			auto mousePosition = m_pInputDevice->getMouseStateComponent()->getPosition();
			LogInfo("Left mouse button pressed at window position (x,y) = (" + std::to_string((int32_t)mousePosition.x()) + ", " + std::to_string((int32_t)mousePosition.y()) + ")");
			m_pInputDevice->getMouseStateComponent()->buttonState(MouseStateComponent::BUTTON_LEFT) = MouseStateComponent::BUTTON_STATE_OFF;
		}
		if (MouseStateComponent::BUTTON_STATE_PRESSED == m_pInputDevice->getMouseStateComponent()->getButtonState(MouseStateComponent::BUTTON_RIGHT)) {
			auto mousePositionDelta = m_pInputDevice->getMouseStateComponent()->getPositionDelta();
			LogInfo("Right mouse button pressed. Mouse Position delta since last click is (x,y) = (" + std::to_string((int32_t)mousePositionDelta.x()) + ", " + std::to_string((int32_t)mousePositionDelta.y()) + ")");
			m_pInputDevice->getMouseStateComponent()->positionDelta() = Eigen::Vector2f::Zero();
			m_pInputDevice->getMouseStateComponent()->buttonState(MouseStateComponent::BUTTON_RIGHT) = MouseStateComponent::BUTTON_STATE_OFF;

		}

		m_frameProcessingCounter += GeneralUtility::timestampHighPrecision() - timestampFrameStart;
		m_pWindowSystem->swapBuffers();
		m_frameCounter++;


		if (GeneralUtility::getTimestamp() - m_lastFpsPrint > 1000) {
			float weight = (GeneralUtility::getTimestamp() - m_lastFpsPrint) / 1000.0f;
			float fps = m_frameCounter * weight;
			float msPerFrame = (m_frameProcessingCounter / fps / 1000.0f);

			std::string frameDisplay = " [" + std::to_string(fps) + " fps - " + std::to_string(msPerFrame) + " ms per frame]";
			m_pMainWindow->getWindowPropertiesComponent()->title() = m_exampleName + frameDisplay;
			WindowEntityController::updateWindowTitle(m_pMainWindow);

			m_lastFpsPrint = GeneralUtility::getTimestamp();
			m_frameCounter = 0;
			m_frameProcessingCounter = 0;

			// set new bgColor
			m_pMainWindow->getWindowPropertiesComponent()->clearColor() = m_bgColors[CrossForgeMath::randRange(uint64_t(0), uint64_t(m_bgColors.size()))];
		}

	}
}