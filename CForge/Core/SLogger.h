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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#pragma once

#include "CForgeObject.h"

namespace CForge {
	/**
	* \brief Logger class to collect exceptions, error messages and general program flow.
	*
	* \note The default error log file is "CForgeLog.txt"
	* 
	* \todo Do full documentation
	* \ingroup Core
	*/

	class CFORGE_API SLogger: public CForgeObject {
	public:

		enum LogType : int8_t {
			LOGTYPE_UNKNOWN = -1,
			LOGTYPE_DEBUG = 0,
			LOGTYPE_ERROR,
			LOGTYPE_INFO,
			LOGTYPE_WARNING,
			LOGTYPE_COUNT,
		};

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
		static void log(const std::string Msg, const std::string Tag = "", LogType Type = LOGTYPE_INFO);

		/**
		* \brief Specify a new log file.
		*
		* \param[in] URI Address and name of the new log file. Can be relative or absolute.
		*/
		static void logFile(const std::string URI, LogType Type, bool ResetFile, bool LogImmediately);

		static std::string logFile(LogType Type);

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

		struct LogEntry {
			std::string Msg;
			std::string Tag;
			LogType Type;
		};

		void printLog(std::string LogFile, const std::vector<LogEntry> *pContent);
		void writeToLog(const std::string Msg, const std::string Filename);
	
		std::string m_ErrorLogFile; ///< Error log file. 
		std::string m_DebugLogFile;
		std::string m_InfoLogFile;
		std::string m_WarningLogFile;

		bool m_LogImmediately[LOGTYPE_COUNT];

		std::vector<LogEntry> m_ErrorLog;
		std::vector<LogEntry> m_DebugLog;
		std::vector<LogEntry> m_InfoLog;
		std::vector<LogEntry> m_WarningLog;
	};//SLogger

	typedef SLogger Logger;
}//name-space