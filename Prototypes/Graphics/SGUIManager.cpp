#include "SGUIManager.h"
#include "../Core/SLogger.h"


namespace CForge {


	uint32_t SGUIManager::m_InstanceClount = 0;
	SGUIManager* SGUIManager::m_pInstance = nullptr;

	SGUIManager* SGUIManager::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new SGUIManager();
			m_pInstance->init();
		}
		m_InstanceClount++;
		return m_pInstance;
	}//instance

	void SGUIManager::release(void) {
		if (m_InstanceClount == 0) throw CForgeExcept("Not enough instance for a release call!");
		m_InstanceClount--;
		if (0 == m_InstanceClount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	SGUIManager::SGUIManager(void) : CForgeObject("SGUIManager") {

	}//Constructor

	SGUIManager::~SGUIManager(void) {

	}//Destructor

	void SGUIManager::init(void) {

	}//initialize


}//name space