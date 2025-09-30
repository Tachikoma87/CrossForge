#include "TriangleMeshIOControllerBase.h"

namespace crossforge {

	TriangleMeshIOControllerBase::TriangleMeshIOControllerBase(std::string childIdentification): ControllerBase(TriangleMeshIOControllerBase::identification) {
		m_inheritance.push_back(childIdentification);

	}
	TriangleMeshIOControllerBase::~TriangleMeshIOControllerBase() {

	}

}