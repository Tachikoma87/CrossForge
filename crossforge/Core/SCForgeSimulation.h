/*****************************************************************************\
*                                                                           *
* File(s): SCForgeSimulation.h and SCForgeSimulation.cpp                    *
*                                                                           *
* Content: Singleton class that holds all information vital to the    *
*          simulation.    *
*                                                 *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SCFORGESIMULATION_H__
#define __CFORGE_SCFORGESIMULATION_H__

#include "CForgeObject.h"

namespace CForge {
	class CFORGE_API SCForgeSimulation {
	public:
		static SCForgeSimulation* instance(void);
		static int64_t simulationTime(void);
		static int64_t simulationDelta(void);

		void release(void);

		int64_t timestamp(void);
		int64_t timeDelta(void);
		void timestamp(int64_t Milliseconds);

		/**
		\brief Advances simulation time by specified amount of milliseconds. If 0 it will advance in real time.
		*/
		void advanceTime(int64_t Milliseconds = 0);

	protected:
		SCForgeSimulation(void);
		~SCForgeSimulation(void);
		void init(void);

		int64_t m_SimulationTimestamp; ///< Timestamp in milliseconds
		int64_t m_SimulationDelta; ///< Difference to last update (typically rendered frame)
		uint64_t m_LastSimulationUpdate; ///< Last simulation update


	private:
		static SCForgeSimulation* m_pInstance;
		static int32_t m_InstanceCount;

	};//SCForgeSimulation

	typedef SCForgeSimulation CForgeSimulation;
}

#endif