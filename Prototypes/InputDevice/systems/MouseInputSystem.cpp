#include <GLFW/glfw3.h>
#include "MouseInputSystem.h"
#include <crossforge/ecs/SEntityManager.h>
#include <crossforge/ecs/SSystemManager.h>
#include "../entities/InputDeviceEntity.h"
#include <crossforge/graphics/entities/WindowEntity.h>

namespace crossforge {

	MouseInputSystem::MouseInputSystem(): SystemBase(MouseInputSystem::identification) {
		m_inheritance.push_back(identification);

	}

	MouseInputSystem::~MouseInputSystem() {

	}

	void MouseInputSystem::initialize() {

	}
	void MouseInputSystem::clear() {

	}
	void MouseInputSystem::update() {
		// all input is handled by the callbacks controlled by glfw, thus nothing to do in the update method
	}
	bool MouseInputSystem::isEntityValid(EntityBasePtr pEntity) const{
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		bool result = true;
		if (!pEntity->isInstanceOf(InputDeviceEntity::identification)) result = false;
		if (!pEntity->hasComponent(AssociatedWindowComponent::identification)) result = false;
		return result;
	}

	bool MouseInputSystem::registerEntity(EntityBasePtr pEntity) {
		bool result = SystemBase::registerEntity(pEntity);
		if (result) {
			auto pInputDevice = EntityBase::cast<InputDeviceEntity>(pEntity);
			auto pWindowEntity = EntityManager::getInstance()->getEntity<WindowEntity>(pInputDevice->getAssociatedWindowComponent()->windowEntityId());
			if (nullptr != pWindowEntity) {
				auto *pHandle = (GLFWwindow*)pWindowEntity->getWindowPropertiesComponent()->getGlfwWindowHandle();
				
				if (!pInputDevice->hasComponent(MouseDataComponent::identification)) pInputDevice->addComponent(std::make_shared<MouseDataComponent>());
				if (m_handelToEntitiesMap.end() == m_handelToEntitiesMap.find(pHandle)) m_handelToEntitiesMap.insert(std::pair((void*)pHandle, std::vector<InputDeviceEntityPtr>()));
				m_handelToEntitiesMap.find(pHandle)->second.push_back(pInputDevice);

				glfwSetMouseButtonCallback(pHandle, MouseInputSystem::mouseButtonCallbackFunc);
				glfwSetScrollCallback(pHandle, MouseInputSystem::mouseWheelCallbackFunc);
				glfwSetCursorPosCallback(pHandle, MouseInputSystem::mousePositionCallbackFunc);
			}
		}
		return result;
	}

#pragma region [static protected methods]

	std::vector<InputDeviceEntityPtr> MouseInputSystem::getDeviceListFromHandle(void* pHandle) {
		std::vector<InputDeviceEntityPtr> result;
		auto pMouseSystem = SystemManager::getInstance()->getSystem<MouseInputSystem>();
		auto inputDeviceList = pMouseSystem->m_handelToEntitiesMap.find(pHandle);
		if (inputDeviceList != pMouseSystem->m_handelToEntitiesMap.end()) {
			result = inputDeviceList->second;
		}
		return result;
	}

	void MouseInputSystem::mousePositionCallbackFunc(struct GLFWwindow* pWin, double xPos, double yPos) {
		Eigen::Vector2f newPos(xPos, yPos);

		auto deviceList = getDeviceListFromHandle(pWin);
		for (auto pInputDevice : deviceList) {
			auto pDataComponent = pInputDevice->getMouseDataComponent();
			Eigen::Vector2f oldPos = pDataComponent->position();
			pDataComponent->positionDelta() += (newPos - oldPos);
			pDataComponent->position() = newPos;
		}
		
	}

	void MouseInputSystem::mouseButtonCallbackFunc(struct GLFWwindow* pWin, int Button, int Action, int Mode) {
		MouseDataComponent::Button btn = MouseDataComponent::BUTTON_UNKNOWN;
		MouseDataComponent::ButtonState btnState = MouseDataComponent::STATE_OFF;

		switch (Button) {
		case GLFW_MOUSE_BUTTON_LEFT: btn = MouseDataComponent::BUTTON_LEFT; break;
		case GLFW_MOUSE_BUTTON_RIGHT: btn = MouseDataComponent::BUTTON_RIGHT; break;
		case GLFW_MOUSE_BUTTON_MIDDLE: btn = MouseDataComponent::BUTTON_MIDDLE; break;
		case GLFW_MOUSE_BUTTON_4: btn = MouseDataComponent::BUTTON_4; break;
		case GLFW_MOUSE_BUTTON_5: btn = MouseDataComponent::BUTTON_5; break;
		case GLFW_MOUSE_BUTTON_6: btn = MouseDataComponent::BUTTON_6; break;
		case GLFW_MOUSE_BUTTON_7: btn = MouseDataComponent::BUTTON_7; break;
		case GLFW_MOUSE_BUTTON_8: btn = MouseDataComponent::BUTTON_8; break;
		default: break;
		}

		switch (Action) {
		case GLFW_PRESS: btnState = MouseDataComponent::STATE_PRESSED; break;
		case GLFW_RELEASE: btnState = MouseDataComponent::STATE_RELEASED; break;
		default: break;
		}

		if (btn == MouseDataComponent::BUTTON_UNKNOWN) return;
		
		auto deviceList = getDeviceListFromHandle(pWin);
		for (auto pInputDevice : deviceList) {
			auto pDataComponent = pInputDevice->getMouseDataComponent();
			pDataComponent->buttonState(btn) = btnState;
		}	
	}
	void MouseInputSystem::mouseWheelCallbackFunc(struct GLFWwindow* pWin, double xOffset, double yOffset) {
		Eigen::Vector2f newOffset(xOffset, yOffset);
		auto deviceList = getDeviceListFromHandle(pWin);
		for (auto pInputDevice : deviceList) {
			auto pDataComponent = pInputDevice->getMouseDataComponent();
			pDataComponent->wheelOffset() = newOffset;	
		}
	}

#pragma endregion
}