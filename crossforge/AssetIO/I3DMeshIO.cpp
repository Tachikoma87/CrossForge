#include "I3DMeshIO.h"

namespace CForge {
	
	std::string I3DMeshIO::pluginName(void)const {
		return m_PluginName;
	}//pluginName


	I3DMeshIO::I3DMeshIO(const std::string ClassName): CForgeObject("I3DMeshIO::" + ClassName) {
		m_PluginName = "3DMeshIO base class";
	}//Constructor

	I3DMeshIO::~I3DMeshIO(void) {
		m_PluginName = "";
	}//Destructor

}//name space