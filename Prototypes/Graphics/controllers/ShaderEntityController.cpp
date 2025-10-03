#include "ShaderEntityController.h"

namespace crossforge {


	ShaderEntityController::ShaderEntityController(): ControllerBase(ShaderEntityController::identification) {

	}
	ShaderEntityController::ShaderEntityController(const std::string childIdentification): ControllerBase(ShaderEntityController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	ShaderEntityController::~ShaderEntityController() {

	}
}