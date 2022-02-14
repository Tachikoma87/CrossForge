#include "SLOD.h"

namespace CForge {
	SLOD* SLOD::m_pInstance = nullptr;
	uint32_t SLOD::m_InstanceCount = 0;

	SLOD* SLOD::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new SLOD();
			m_pInstance->init();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void SLOD::release(void) {
		if (m_InstanceCount == 0) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	std::vector<float>* SLOD::getLevels() {
		return &LODLevels;
	}

	void SLOD::update() {
		auto start = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::high_resolution_clock::now() - this->lastTickCount;
		this->lastTickCount = start;
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		this->deltaTime = 0.000001 * microseconds;
	}

	void SLOD::setResolution() {

	}

	SLOD::SLOD(void){
		this->init();
	}//Constructor

	SLOD::~SLOD(void) {
		
	}//Destructor

	void SLOD::init(void) {
		this->deltaTime = 1.0/60.0;
		//TODO set reolution with param
		//this->resolution = 
	}//initialize

	void SLOD::clear(void) {

	}//initialize
}