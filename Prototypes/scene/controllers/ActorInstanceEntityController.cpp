#include "ActorInstanceEntityController.h"

namespace crossforge {

	ActorInstanceEntityController::ActorInstanceEntityController(const std::string childIdentification): ControllerBase(ActorInstanceEntityController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	ActorInstanceEntityController::~ActorInstanceEntityController() {

	}

	
}