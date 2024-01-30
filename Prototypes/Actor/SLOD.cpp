#include "SLOD.h"

#include <crossforge/Graphics/OpenGLHeader.h>

namespace CForge {
	SLOD* SLOD::m_pInstance = nullptr;
	uint32_t SLOD::m_InstanceCount = 0;

	SLOD* SLOD::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new SLOD();
			m_pInstance->init();
			m_pInstance->update();
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
		auto start = std::chrono::steady_clock::now();
		auto elapsed = start - this->lastTickCount;
		this->lastTickCount = start;
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		this->deltaTime = 0.000001 * microseconds;
	}

	double SLOD::getDeltaTime() {
		return deltaTime;
	}

	void SLOD::setResolution(Eigen::Vector2i resolution) {
		this->resolution = resolution;
		res_pixAmount = resolution.x() * resolution.y();
	}

	uint32_t SLOD::getResPixAmount() {
		return res_pixAmount;
	}
	
	std::vector<float> SLOD::getLODPercentages() {
		return LODPercentages;
	}

	SLOD::SLOD(void){
		this->init();
	}//Constructor

	SLOD::~SLOD(void) {
		
	}//Destructor

	void SLOD::init() {
		this->deltaTime = 1.0/60.0;
	}//initialize

	void SLOD::clear(void) {

	}//initialize

	void SLOD::setLODLevels(std::vector<float> levels) {
		LODLevels = levels;
	}
}
