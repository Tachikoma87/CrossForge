#include "SLogger.h"
#include "../AssetIO/File.h"
#include "../Math/CForgeMath.h"
#include "../Utility/CForgeUtility.h"

using namespace std;

namespace CForge {

	SLogger* SLogger::m_pInstance = nullptr;
	int16_t SLogger::m_InstanceCount = 0;

	void SLogger::logException(const CrossForgeException Except) {		
		std::string Msg = "Exception occurred at " + Except.function() + "(L " + std::to_string(Except.line()) + "): " + Except.msg() + "\n";
		log(Msg, "Exception", LOGTYPE_ERROR);
	}//logException

	void SLogger::log(const std::string Msg, const string Tag, LogType Type) {
		if (nullptr == m_pInstance) throw NotInitializedExcept("SLogger not initialized");

#if defined(__EMSCRIPTEN__)
		std::string ErrorMsg = Tag + ":" + Msg;
		printf("%s\n", ErrorMsg.c_str());
#endif

		LogEntry Entry;
		Entry.Msg = Msg;
		Entry.Tag = Tag;
		Entry.Type = Type;

		switch (Type) {
		case LOGTYPE_ERROR: {
			m_pInstance->m_ErrorLog.push_back(Entry);
			if (m_pInstance->m_LogImmediately[LOGTYPE_ERROR]) m_pInstance->writeToLog(Tag + ":" + Msg + "\n", m_pInstance->m_ErrorLogFile);
		}break;
		case LOGTYPE_DEBUG: {
			m_pInstance->m_DebugLog.push_back(Entry);
			if (m_pInstance->m_LogImmediately[LOGTYPE_DEBUG]) m_pInstance->writeToLog(Tag + ":" + Msg + "\n", m_pInstance->m_DebugLogFile);
		}break;
		case LOGTYPE_INFO: {
			m_pInstance->m_InfoLog.push_back(Entry);
			if (m_pInstance->m_LogImmediately[LOGTYPE_INFO]) m_pInstance->writeToLog(Tag + ":" + Msg + "\n", m_pInstance->m_InfoLogFile);
		}break;
		case LOGTYPE_WARNING: {
			m_pInstance->m_WarningLog.push_back(Entry);
			if (m_pInstance->m_LogImmediately[LOGTYPE_WARNING]) m_pInstance->writeToLog(Tag + ":" + Msg + "\n", m_pInstance->m_WarningLogFile);
		}break;
		default: {
			throw CForgeExcept("Invalid log type specified!");
		}break;
		}//switch[Type]
	}//log

	void SLogger::logFile(const string URI, LogType Type, bool ResetFile, bool LogImmediately ) {
		if (nullptr == m_pInstance) throw NotInitializedExcept("SLogger not initialized!");

		if (ResetFile) {
			File F;
			F.begin(URI, "w");
			F.end();
		}

		switch (Type) {
		case LOGTYPE_ERROR: {
			m_pInstance->m_ErrorLogFile = URI;
		}break;
		case LOGTYPE_DEBUG: {
			m_pInstance->m_DebugLogFile = URI;
		}break;
		case LOGTYPE_INFO: {
			m_pInstance->m_InfoLogFile = URI;
		}break;
		case LOGTYPE_WARNING: {
			m_pInstance->m_WarningLogFile = LOGTYPE_WARNING;
		}break;
		default: throw CForgeExcept("Invalid log type specified!");
		}
		m_pInstance->m_LogImmediately[Type] = LogImmediately;

	}//outputFile

	SLogger* SLogger::instance(void) {
		if (nullptr == m_pInstance) m_pInstance = new SLogger();
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void SLogger::release(void) {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	void SLogger::writeToLog(const std::string Msg, const std::string Filename) {
		File F;
		F.begin(Filename, "ab");
		if (F.valid()) F.write(Msg.c_str(), Msg.size());
		F.end();
	}//writeToLog


	SLogger::SLogger(void): CForgeObject("SLogger") {
		m_ErrorLogFile = "CForgeLog.txt";
		m_DebugLogFile = "CForgeLog.txt";
		m_InfoLogFile = "CForgeLog.txt";
		m_WarningLogFile = "CForgeLog.txt";

		CForgeUtility::memset(&m_LogImmediately[0], true, LOGTYPE_COUNT);

		m_ErrorLog.clear();
		m_DebugLog.clear();
		m_InfoLog.clear();
		m_WarningLog.clear();
	}//Constructor

	SLogger::~SLogger(void) {
		// log everything if requested
		if (!m_LogImmediately[LOGTYPE_ERROR] && !m_ErrorLogFile.empty()) printLog(m_ErrorLogFile, &m_ErrorLog);
		if (!m_LogImmediately[LOGTYPE_DEBUG] && !m_DebugLogFile.empty()) printLog(m_DebugLogFile, &m_DebugLog);
		if (!m_LogImmediately[LOGTYPE_INFO] && !m_InfoLogFile.empty()) printLog(m_InfoLogFile, &m_InfoLog);
		if (!m_LogImmediately[LOGTYPE_WARNING] && !m_WarningLogFile.empty()) printLog(m_WarningLogFile, &m_WarningLog);

	}//Destructor


	std::string SLogger::logFile(LogType Type) {
		std::string Rval = "";
		switch (Type) {
		case LOGTYPE_ERROR: Rval = m_pInstance->m_ErrorLogFile; break;
		case LOGTYPE_WARNING: Rval = m_pInstance->m_WarningLogFile; break;
		case LOGTYPE_INFO: Rval = m_pInstance->m_WarningLogFile; break;
		case LOGTYPE_DEBUG: Rval = m_pInstance->m_DebugLogFile; break;
		default: throw CForgeExcept("Invalid log type specified!");
		}
		return Rval;
	}//logFile

	void SLogger::printLog(std::string LogFile, const std::vector<LogEntry>* pContent) {
		File F;
		F.begin(LogFile, "ab");
		if (!F.valid()) {
			printf("Unable to open file: %s. Can not write log.\n", m_ErrorLogFile.c_str());
		}
		else {
			for (auto i : (*pContent)) {
				const string Msg = i.Tag + ":" + i.Msg + "\n";
				F.write(Msg.c_str(), Msg.size());
			}//for[error log]
		}
		F.end();
	}//printLog

}//name-space