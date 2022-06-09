#include "I3DHairIO.h"

namespace CForge {

	std::string I3DHairIO::pluginName(void)const {
		return m_PluginName;
	}//pluginName


	I3DHairIO::I3DHairIO(const std::string ClassName): CForgeObject("I3DHairIO::" + ClassName) {
		m_PluginName = "3DHairIO base class";
	}//Constructor

	I3DHairIO::~I3DHairIO(void) {
		m_PluginName = "";
	}//Destructor

}//name space