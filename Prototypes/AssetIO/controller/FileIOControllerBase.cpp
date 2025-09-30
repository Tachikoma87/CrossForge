#include "FileIOControllerBase.h"

namespace crossforge {
	FileIOControllerBase::FileIOControllerBase(const std::string identification): ControllerBase(FileIOControllerBase::identification) {
		m_inheritance.push_back(identification);
	}
	FileIOControllerBase::~FileIOControllerBase() {

	}
}