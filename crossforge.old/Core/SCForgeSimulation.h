/*****************************************************************************\
*                                                                           *
* File(s): SCForgeSimulation.h and SCForgeSimulation.cpp                    *
*                                                                           *
* Content: Singleton class that holds all information vital to the          *
*          simulation.                                                      *
*                                                                           *
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
	/**
	* \brief The simulation class is used to decouple the program's internal time from the real clock time. All animation steps should be based on the time information provided by this class. 
	* \ingroup Core
	* 
	* \todo Write tutorial page on the simulation concept.
	*/
	class CFORGE_API SCForgeSimulation {
	public:
		/**
		* \brief Singleton instantiation method.
		* \return Class's instance.
		*/
		static SCForgeSimulation* instance(void);

		/**
		* \brief Getter for number of active instances.
		*/
		static int32_t instaceCount();

		/**
		* \brief Getter for the simulation time.
		* \return Timestamp of the simulation in milliseconds.
		*/
		static int64_t simulationTime(void);

		/**
		* \brief Getter for the simulation time delta, i.e. time delta to last simulation update.
		* \return Time delta to last simulation update in milliseconds.
		*/
		static int64_t simulationTimeDelta(void);

		/**
		* \brief Singleton's release method.
		*/
		void release(void);

		/**
		* \brief Getter for the timesamp of the simulation.
		* \return Timestamp of the simulation in milliseconds.
		*/
		int64_t timestamp(void);

		/**
		* \brief Getter for the simulation time delta, i.e. time delta to last simulation update.
		* \return Time delta to last simulation update in milliseconds.
		*/
		int64_t timeDelta(void);

		/**
		* \brief Setter for the timestamp.
		* \param[in] Milliseconds New timestamp in milliseconds.
		* \param[in] New time delta in milliseconds.
		*/
		void timestamp(int64_t Milliseconds, int64_t TimeDelta = 0);

		/**
		* \brief Advances simulation time by specified amount of milliseconds. If 0 it will advance in real time. If negative time will go backwards.
		* \param[in] Milliseconds Amount of time in milliseconds to advance the simulation. If 0 time will advance according to system clock.
		*/
		void advanceTime(int64_t Milliseconds = 0);

	protected:
		/**
		* \brief Constructor
		*/
		SCForgeSimulation(void);

		/**
		* \brief Destructor
		*/
		~SCForgeSimulation(void);

		/**
		* \brief Initialization method.
		*/
		void init(void);

		int64_t m_SimulationTimestamp;		///< Simulation timestamp in milliseconds.
		int64_t m_SimulationDelta;			///< Difference to last update (typically rendered frame).
		uint64_t m_LastSimulationUpdate;	///< Timestamp of last simulation update of the system's clock.

	private:
		static SCForgeSimulation* m_pInstance;	///< Instance pointer.
		static int32_t m_InstanceCount;			///< Instance counter.

	};//SCForgeSimulation

	typedef SCForgeSimulation CForgeSimulation;	///< Quality of life definition.
}

#endif