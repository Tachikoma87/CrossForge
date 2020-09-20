#include "SLogger.h"
#include "File.h"

namespace CForge {

	SLogger* SLogger::m_pInstance = nullptr;
	int16_t SLogger::m_InstanceCount = 0;

	void SLogger::logException(const CrossForgeException Except) {		
		std::string Msg = "Exception occurred at " + Except.function() + "(L " + std::to_string(Except.line()) + "): " + Except.msg() + "\n";
		log(Msg);	
	}//logException

	void SLogger::log(const std::string Msg) {
		if (nullptr == m_pInstance) throw NotInitializedExcept("SLogger not initialized!");
		File F;
		F.begin(m_pInstance->m_ErrorLogFile, "ab");
		F.write(Msg.c_str(), Msg.length());
		F.end();
	}//log

	void SLogger::logFile(const std::string URI) {
		if (nullptr == m_pInstance) throw NotInitializedExcept("SLogger not initialized!");
		m_pInstance->m_ErrorLogFile = URI;
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

	SLogger::SLogger(void) {
		m_ErrorLogFile = "CForgeLog.txt";
	}//Constructor

	SLogger::~SLogger(void) {
		m_ErrorLogFile = "";
	}//Destructor


}//name-space