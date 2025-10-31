/*****************************************************************************\
*                                                                           *
* File(s): SLogger.h and SLogger.cpp                                        *
*                                                                           *
* Content: A simple logger class that can write error messages and          *
*          exceptions to an error log file.                                 *
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
	* \ingroup Core
	* \note The default error log file is "CForgeLog.txt"
	*  
	* \todo Write documentation concept page about logging.
	* \todo Change output file type to *.md and create a well-arranged log. PrintFullLog or something which prints all messages in a structured way.
	* \todo Add methods to make log messages accessible (Getter).
	*/
	class CFORGE_API SLogger: public CForgeObject {
	public:

		/**
		* \brief Defines for the available log types.
		*/
		enum LogType : int8_t {
			LOGTYPE_UNKNOWN = -1,	///< Default value.
			LOGTYPE_DEBUG = 0,		///< Debug log.
			LOGTYPE_ERROR,			///< Error log.
			LOGTYPE_INFO,			///< Info log.
			LOGTYPE_WARNING,		///< Warning log.
			LOGTYPE_COUNT,			///< Number of log types.
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
		* \param[in] Tag Optional tag for the message.
		*/
		static void log(const std::string Msg, const std::string Tag = "", LogType Type = LOGTYPE_INFO);


		static void log(const std::string msg, const std::string Tag, LogType Type, std::string file, int32_t line, std::string functionName);

		/**
		* \brief Specify a new log file.
		*
		* \param[in] URI Address and name of the new log file. Can be relative or absolute.
		* \param[in] Type Which types of messages should be written to this file.
		* \param[in] ResetFile Whether or not the existing file should be reset.
		* \param[in] Whether or not messages of the specified type should be written immediately to the file.
		*/
		static void logFile(const std::string URI, LogType Type, bool ResetFile, bool LogImmediately);

		/**
		* \brief Getter for the assigned log file.
		* 
		* \param[in] Type Log type to get the assigned file for.
		* \return Path to the assigned log file. Empty if no file was assigned.
		*/
		static std::string logFile(LogType Type);

		/**
		* \brief Singleton instantiation method.
		* 
		* \return Pointer to the unique instance.
		*/
		static SLogger* instance(void);

		/**
		* \brief Singleton release method. Call once for every instance call.
		*/
		void release(void);

		/**
		* \brief Getter for number of active instances.
		* 
		* \return Number of active instances.
		*/
		static int32_t instanceCount(void);

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

		/**
		* \brief Structure of a log entry.
		*/
		struct LogEntry {
			std::string Msg;	///< The message.
			std::string Tag;	///< Message tag.
			LogType Type;		///< Type.
		};

		/**
		* \brief Attaches the specified log entries to the specified log file.
		* 
		* \param[in] LogFile Log file to write to.
		* \param[in] pContent Log entries to add.
		*/
		void printLog(std::string LogFile, const std::vector<LogEntry> *pContent);

		/**
		* \brief Adds a the specified string to the log file.
		* 
		* \param[in] LogFile File to write to.
		* \param[in] Msg Message to write.
		*/
		void writeToLog(const std::string LogFile, const std::string Msg);
	
		std::string m_ErrorLogFile;		///< Error log file. 
		std::string m_DebugLogFile;		///< Debug log file.
		std::string m_InfoLogFile;		///< Info log file.
		std::string m_WarningLogFile;	///< Warning log file.

		bool m_LogImmediately[LOGTYPE_COUNT];	///< Array specifying whether messages should be logged immediately when received.

		std::vector<LogEntry> m_ErrorLog;	///< Entries of the error log.
		std::vector<LogEntry> m_DebugLog;	///< Entries of the debug log.
		std::vector<LogEntry> m_InfoLog;	///< Entries of the info log.
		std::vector<LogEntry> m_WarningLog;	///< Entries of the warning log.
	};//SLogger

	typedef SLogger Logger;	///< Convenience definition.

#ifdef WIN32
#define LogInfo(Msg, Tag) CForge::SLogger::log(Msg, Tag, SLogger::LOGTYPE_INFO, __FILE__, __LINE__, __FUNCTION__) 
#define LogDebug(Msg, Tag) CForge::SLogger::log(Msg, Tag, SLogger::LOGTYPE_DEBUG, __FILE__, __LINE__, __FUNCTION__) 
#define LogWarning(Msg, Tag) CForge::SLogger::log(Msg,Tag, SLogger::LOGTYPE_WARNING, __FILE__, __LINE__, __FUNCTION__) 
#define LogError(Msg, Tag) CForge::SLogger::log(Msg, Tag, SLogger::LOGTYPE_ERROR, __FILE__, __LINE__, __FUNCTION__) 

#else
//#define CForgeExcept(Msg) CForge::CrossForgeException(CForge::CrossForgeException::Type::GENERAL, Msg, "", "", __FILE__, __LINE__, __PRETTY_FUNCTION__)
//#define NullpointerExcept(Variable) CForge::CrossForgeException(CForge::CrossForgeException::Type::NULLPOINTER, Variable, "", "", __FILE__, __LINE__, __PRETTY_FUNCTION__)
//#define NotInitializedExcept(What) CForge::CrossForgeException(CForge::CrossForgeException::Type::NOT_INITIALIZED, What, "", "", __FILE__, __LINE__, __PRETTY_FUNCTION__)
//#define IndexOutOfBoundsExcept(Variable) CForge::CrossForgeException(CForge::CrossForgeException::Type::INDEX_OUT_OF_BOUNDS, Variable, "", "", __FILE__,__LINE__, __PRETTY_FUNCTION__)
//#define OutOfMemoryExcept(Variable) CForge::CrossForgeException(CForge::CrossForgeException::Type::OUT_OF_MEMORY, Variable, "", "", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif
	

	
}//name-space