#include "MiscellaneousController.h"
#include <GLFW/glfw3.h>

namespace crossforge {

	MiscellaneousController::MiscellaneousController(const std::string childIdentification): ControllerBase(MiscellaneousController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	MiscellaneousController::~MiscellaneousController() {

	}


	bool MiscellaneousController::updateWindowTitle(WindowEntityPtr pWindow) {
		if (nullptr == pWindow) throw NullpointerExcept("pWindow");
		auto pWindowPropComp = pWindow->getWindowPropertiesComponent();
		if (nullptr == pWindowPropComp) throw MissingComponentException(WindowPropertiesComponent::identification);
		bool result = false;

		auto pHandle = (GLFWwindow*)pWindowPropComp->getGlfwWindowHandle();
		if (nullptr == pHandle) {
			LogError("Window handle is nullptr.");
		}else{
			glfwSetWindowTitle(pHandle, pWindowPropComp->title().c_str());
			result = true;
		}
		return result;
	}

	bool MiscellaneousController::updateVerticalSynchronization(WindowEntityPtr pWindow) {
		if (nullptr == pWindow) throw NullpointerExcept("pWindow");
		auto pWindowPropComp = pWindow->getWindowPropertiesComponent();
		if (nullptr == pWindowPropComp) throw MissingComponentException(WindowPropertiesComponent::identification);
		bool result = false;

		auto pHandle = (GLFWwindow*)pWindowPropComp->getGlfwWindowHandle();
		if (nullptr == pHandle) {
			LogError("Window handle is nullptr");
		}
		else {
			glfwSwapInterval(pWindowPropComp->vSyncInterval());
			result = true;
		}
		return result;

	}
}