#include "VideoController.h"

namespace crossforge {

	VideoController::VideoController(const std::string identification) : ControllerBase(VideoController::identification) {
		m_inheritance.push_back(identification);
	}

	VideoController::~VideoController() {

	}

}