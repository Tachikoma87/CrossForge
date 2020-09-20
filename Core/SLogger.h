/*****************************************************************************\
*                                                                           *
* File(s): SLogger.h and SLogger.cpp                                        *
*                                                                           *
* Content: A simple logger class that can write error messages and          *
*          exceptions to an error log file.                                  *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#pragma once

#include <inttypes.h>
#include <string>
#include "CrossForgeException.h"

namespace CForge {
	/**
	* \brief Logger class to collect exceptions, error messages and general program flow.
	*
	* \note The default error log file is "CForgeLog.txt"
	* \todo Extend class with particular log files (program flow)
	* \ingroup Core
	*/

	class SLogger {
	public:
		/**
		* \brief Log a Cross forge exception. Based on the exception type a specific error message will be written to the error leg.
		*
		* \param[in] Except The Exception to log.
		*/
		static void logException(const CrossForgeException Except);

		/**
		* \brief Log a message. Message will be written to the error log.
		*
		* \param[in] Msg Message to log.
		*/
		static void log(const std::string Msg);

		/**
		* \brief Specify a new log file.
		*
		* \param[in] URI Address and name of the new log file. Can be relative or absolute.
		*/
		static void logFile(const std::string URI);

		/**
		*\brief Instantiation method.
		* \return Pointer to the unique instance.
		*/
		static SLogger* instance(void);

		/**
		*\brief Release method. Call once for every instance call.
		*/
		void release(void);

	protected:
		/**
		*\brief Constructor.
		*/
		SLogger(void);

		/**
		*\brief Destructor
		*/
		~SLogger(void);
	private:
		static SLogger* m_pInstance;	///< Holds the unique instance pointer.
		static int16_t m_InstanceCount; ///< Number of instance calls. If down to zero the object gets destroyed.

		std::string m_ErrorLogFile; ///< Error log file. 

	};//SLogger

	typedef SLogger Logger;
}//name-space