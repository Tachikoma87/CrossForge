#include "SCrossForgeDevice.h"
#include "CForgeObject.h"

namespace CForge {

	CForgeObject::CForgeObject(const std::string ClassName) {
		m_ClassName = ClassName;
		SCrossForgeDevice *pMainDevice = SCrossForgeDevice::instance();
		m_ObjectID = pMainDevice->registerObject(this);
		pMainDevice->release();
	}//Constructor

	CForgeObject::~CForgeObject(void) {
		SCrossForgeDevice* pMainDevice = SCrossForgeDevice::instance();
		pMainDevice->unregisterObject(this);
		pMainDevice->release();

		m_ObjectID = INVALID_ID;
		m_ClassName = "";
	}//Destructor

	uint32_t CForgeObject::objectID(void)const {
		return m_ObjectID;
	}//objectID

	std::string CForgeObject::className(void)const {
		return m_ClassName;
	}//className

}//name space