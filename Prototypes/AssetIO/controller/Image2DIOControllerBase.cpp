#include "Image2DIOControllerBase.h"

namespace crossforge {

	Image2DIOControllerBase::Image2DIOControllerBase(const std::string childIdentification): ControllerBase(Image2DIOControllerBase::identification) {
		m_inheritance.push_back(childIdentification);
	}
	Image2DIOControllerBase::~Image2DIOControllerBase() {

	}

}