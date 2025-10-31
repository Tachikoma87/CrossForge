#include "LightEntityController.h"

namespace crossforge {

	LightEntityController::LightEntityController(): ControllerBase(LightEntityController::identification) {

	}
	LightEntityController::LightEntityController(const std::string childIdentification): ControllerBase(LightEntityController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	LightEntityController::~LightEntityController() {

	}

}