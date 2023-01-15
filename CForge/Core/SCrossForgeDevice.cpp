#ifdef WIN32
#include <WinSock2.h>
#endif
#include <GLFW/glfw3.h>
#include "SCrossForgeDevice.h"
#include "SLogger.h"
#include "SGPIO.h"
#include "../AssetIO/SAssetIO.h"
#include "../Graphics/STextureManager.h"
#include "../Graphics/Shader/SShaderManager.h"


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

		#ifdef WIN32
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			SLogger::log("Error initializing wsa" + std::to_string(WSAGetLastError()), "SCrossForgeDevice", SLogger::LOGTYPE_ERROR);
		}
		#endif

		m_RegisteredObjects.clear();
		m_FreeObjSlots.clear();

		m_pLogger = SLogger::instance();
		m_pAssIO = SAssetIO::instance();
		m_pSMan = SShaderManager::instance();
		m_pTexMan = STextureManager::instance();


#ifdef _WIN32
		m_pGPIO = nullptr;
#else
		m_pGPIO = SGPIO::instance();
#endif

		
	}//initialize

	void SCrossForgeDevice::clear(void) {
		string MemLeakFile = "";
		if (nullptr != m_pGPIO) m_pGPIO->release();
		if (nullptr != m_pAssIO) m_pAssIO->release();
		if (nullptr != m_pTexMan) m_pTexMan->release();
		if (nullptr != m_pSMan) m_pSMan->release();
		if (nullptr != m_pLogger) {
			MemLeakFile = SLogger::logFile(SLogger::LOGTYPE_DEBUG);
			m_pLogger->release();
		}

		m_pLogger = nullptr;
		m_pGPIO = nullptr;
		m_pAssIO = nullptr;
		m_pTexMan = nullptr;
		m_pSMan = nullptr;

		// log all objects that have not been release properly
		uint32_t UnreleasedObjects = 0;
		string MemleakMsg = "";
		for (uint32_t i = 0; i < m_RegisteredObjects.size(); ++i) {
			if (m_RegisteredObjects[i] != nullptr) {
				UnreleasedObjects++;
				CForgeObject* pObj = m_RegisteredObjects[i];
				MemleakMsg += "Object with internal class name \"" + pObj->className() + "\"[ID: " + to_string(pObj->objectID()) + "] was not released properly!\n";
			}
		}//for[registered objects]

		if (0 == UnreleasedObjects) printf("All CForge objects have been released properly!\n");
		else {
			printf("A total of %d objects have not been released properly. See log for details!\n", UnreleasedObjects);
		}
		if(!MemLeakFile.empty()){
			FILE* pF = fopen(MemLeakFile.c_str(), "a");
			//fopen_s(&pF, MemLeakFile.c_str(), "a");
			if (nullptr != pF) {
				fprintf(pF, "%s", MemleakMsg.c_str());
				fclose(pF);
				pF = nullptr;
			}
		}

		m_FreeObjSlots.clear();
		m_RegisteredObjects.clear();

	}//shutdown

	uint32_t SCrossForgeDevice::registerObject(CForgeObject* pObj) {
		if (nullptr == pObj) throw NullpointerExcept("pObj");
		m_Mutex.lock();
		uint32_t Rval = CForgeObject::INVALID_ID;

		if (!m_FreeObjSlots.empty()) {
			Rval = m_FreeObjSlots.back();
			m_FreeObjSlots.pop_back();
			m_RegisteredObjects[Rval] = pObj;
		}
		if (CForgeObject::INVALID_ID == Rval) {
			Rval = m_RegisteredObjects.size();
			m_RegisteredObjects.push_back(pObj);
		}

		m_Mutex.unlock();

		return Rval;
	}//registerObject

	void SCrossForgeDevice::unregisterObject(CForgeObject* pObj) {
		if (nullptr == pObj) throw NullpointerExcept("pObj");
		m_Mutex.lock();


		uint32_t Index = pObj->objectID();
		if (Index >= m_RegisteredObjects.size()) {
			m_Mutex.unlock();
			throw IndexOutOfBoundsExcept("Index of registered object!");
		}

		m_RegisteredObjects[Index] = nullptr;
		m_FreeObjSlots.push_back(Index);

		m_Mutex.unlock();
	}//unregisterObject

	

}//name-space
