#include "MiscellaneousController.h"
#include <GLFW/glfw3.h>

namespace crossforge {

	MiscellaneousController::MiscellaneousController(const std::string childIdentification): ControllerBase(MiscellaneousController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	MiscellaneousController::~MiscellaneousController() {

	}

}