#include "I2DImageIO.h"

namespace CForge {

	I2DImageIO::I2DImageIO(const std::string ClassName): CForgeObject("I2DImageIO::" + ClassName) {
		m_PluginName = "I2DImageIO base class";
	}//Constructor

	I2DImageIO::~I2DImageIO(void) {
		m_PluginName = "";
	}//Destructor

	std::string I2DImageIO::pluginName(void)const {
		return m_PluginName;
	}//pluginName



}//name space