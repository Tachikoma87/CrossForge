#include "SMediaDeviceManager.h"

namespace CForge {
	SMediaDeviceManager* SMediaDeviceManager::m_pInstance = nullptr;
	uint32_t SMediaDeviceManager::m_InstanceCount = 0;

	SMediaDeviceManager* SMediaDeviceManager::instance() {
		if (nullptr == m_pInstance) {
			m_pInstance = new SMediaDeviceManager();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void SMediaDeviceManager::release() {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	SMediaDeviceManager::SMediaDeviceManager() {

	}//Constructor

	SMediaDeviceManager::~SMediaDeviceManager() {

	}//Destructor

	void SMediaDeviceManager::clear() {

	}//clear

	void SMediaDeviceManager::init() {

	}//initialize

}//name space