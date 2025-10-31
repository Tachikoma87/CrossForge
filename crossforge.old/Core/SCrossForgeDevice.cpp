#ifdef WIN32
#include <WinSock2.h>
#endif
#include <GLFW/glfw3.h>

#include "SCrossForgeDevice.h"
#include "SLogger.h"
#include "SGPIO.h"
#include "SCForgeSimulation.h"
#include "../AssetIO/SAssetIO.h"
#include "../Graphics/Textures/STextureManager.h"
#include "../Graphics/Shader/SShaderManager.h"
#include "../Graphics/Font/SFontManager.h"

using namespace std;

namespace CForge {
	SCrossForgeDevice* SCrossForgeDevice::m_pInstance = nullptr;
	int16_t SCrossForgeDevice::m_InstanceCount = 0;

	SCrossForgeDevice* SCrossForgeDevice::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new SCrossForgeDevice();
			m_InstanceCount++;
			m_pInstance->init();
		}
		else {
			m_InstanceCount++;
		}
		
		return m_pInstance;
	}//instance

	int32_t SCrossForgeDevice::instanceCount() {
		return m_InstanceCount;
	}//instanceCount

	void SCrossForgeDevice::release(void) {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		if (1 == m_InstanceCount) {
			m_pInstance->clear();
			delete m_pInstance;
			m_pInstance = nullptr;
			m_InstanceCount = 0;
		}
		else {
			m_InstanceCount--;
		}	
	}//release

	SCrossForgeDevice::SCrossForgeDevice(void) {
		m_pLogger = nullptr;
		m_pAssIO = nullptr;
		m_pGPIO = nullptr;
		m_pSMan = nullptr;
		m_pTexMan = nullptr;
		m_pFontMan = nullptr;
		m_pSimulation = nullptr;
	}//Constructor

	SCrossForgeDevice::~SCrossForgeDevice(void) {	
		// cleanup duty handled in clear
		glfwTerminate();

		#ifdef WIN32
		// clean WSA
		WSACleanup();
		#endif
	}//Destructor

	void SCrossForgeDevice::init(void) {
		glfwInit();

		#if defined(WIN32) && !defined(__EMSCRIPTEN__)
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			SLogger::log("Error initializing wsa" + std::to_string(WSAGetLastError()), "SCrossForgeDevice", SLogger::LOGTYPE_ERROR);
		}
		#endif

		m_pLogger = SLogger::instance();
		m_pAssIO = SAssetIO::instance();
		m_pSMan = SShaderManager::instance();
		m_pTexMan = STextureManager::instance();
		m_pFontMan = SFontManager::instance();
		m_pSimulation = SCForgeSimulation::instance();

#if defined(__linux_) && defined(__arm__)
		m_pGPIO = SGPIO::instance();;
#else
		m_pGPIO = nullptr;
#endif

		
	}//initialize

	void SCrossForgeDevice::clear(void) {
		if (nullptr != m_pGPIO) m_pGPIO->release();
		if (nullptr != m_pAssIO) m_pAssIO->release();
		if (nullptr != m_pTexMan) m_pTexMan->release();
		if (nullptr != m_pSMan) m_pSMan->release();
		if (nullptr != m_pFontMan) m_pFontMan->release();
		if (nullptr != m_pSimulation) m_pSimulation->release();
		if (nullptr != m_pLogger) m_pLogger->release();

		m_pLogger = nullptr;
		m_pGPIO = nullptr;
		m_pAssIO = nullptr;
		m_pTexMan = nullptr;
		m_pSMan = nullptr;
		m_pSimulation = nullptr;

	}//shutdown

}//name-space
