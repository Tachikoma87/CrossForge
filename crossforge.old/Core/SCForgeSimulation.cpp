#include "SCForgeSimulation.h"
#include "../Utility/CForgeUtility.h"

namespace CForge {

	SCForgeSimulation* SCForgeSimulation::m_pInstance = nullptr;
	int32_t SCForgeSimulation::m_InstanceCount = 0;

	SCForgeSimulation* SCForgeSimulation::instance(void) {
		if (m_pInstance == nullptr) {
			m_pInstance = new SCForgeSimulation();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	int32_t SCForgeSimulation::instaceCount() {
		return m_InstanceCount;
	}//instaceCount

	int64_t SCForgeSimulation::simulationTime(void) {
		auto* pInstance = SCForgeSimulation::instance();
		int64_t Rval = pInstance->timestamp();
		pInstance->release();
		return Rval;
	}//timestamp

	int64_t SCForgeSimulation::simulationTimeDelta(void) {
		auto* pInstance = SCForgeSimulation::instance();
		int64_t Rval = pInstance->timeDelta();
		pInstance->release();
		return Rval;
	}//simulationDelta

	void SCForgeSimulation::release(void) {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	int64_t SCForgeSimulation::timestamp(void) {
		return m_SimulationTimestamp;
	}//timestamp

	int64_t SCForgeSimulation::timeDelta(void) {
		return m_SimulationDelta;
	}//timeDelta

	void SCForgeSimulation::timestamp(int64_t Milliseconds, int64_t TimeDelta) {
		m_SimulationTimestamp = Milliseconds;
		m_SimulationDelta = TimeDelta;
		m_LastSimulationUpdate = CForgeUtility::timestamp();
	}//timestamp

	void SCForgeSimulation::advanceTime(int64_t Milliseconds) {
		if (0 == Milliseconds) {
			const uint64_t Stamp = CForgeUtility::timestamp();
			m_SimulationDelta = (Stamp - m_LastSimulationUpdate);
			m_SimulationTimestamp += m_SimulationDelta;
			m_LastSimulationUpdate = Stamp;
		}
		else {
			m_SimulationTimestamp += Milliseconds;
			m_LastSimulationUpdate = CForgeUtility::timestamp();
			m_SimulationDelta = Milliseconds;
		}
	}//advanceTime

	SCForgeSimulation::SCForgeSimulation(void) {
		init();
	}//Constructor

	SCForgeSimulation::~SCForgeSimulation(void) {
		// nothing to do here
	}//Destructor

	void SCForgeSimulation::init(void) {
		m_SimulationTimestamp = 0;
		m_LastSimulationUpdate = CForgeUtility::timestamp();
	}//initialize

}//name-space