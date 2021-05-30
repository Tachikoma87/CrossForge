#include "SCrossForgeDevice.h"
#include "SLogger.h"
#include "../Prototypes/SGPIO.h"

namespace CForge {
	SCrossForgeDevice* SCrossForgeDevice::m_pInstance = nullptr;
	int16_t SCrossForgeDevice::m_InstanceCount = 0;

	SCrossForgeDevice* SCrossForgeDevice::instance(void) {
		if (nullptr == m_pInstance) m_pInstance = new SCrossForgeDevice();
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void SCrossForgeDevice::release(void) {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	SCrossForgeDevice::SCrossForgeDevice(void) {
		m_pLogger = SLogger::instance();
#ifdef _WIN32
		m_pGPIO = nullptr;
#else
		m_pGPIO = SGPIO::instance();
#endif
		
	}//Constructor

	SCrossForgeDevice::~SCrossForgeDevice(void) {	
		if(nullptr != m_pGPIO) m_pGPIO->release();
		m_pLogger->release();
		m_pLogger = nullptr;
		m_pGPIO = nullptr;
	}//Destructor

	

}//name-space