/*****************************************************************************\
*                                                                           *
* File(s): I3DHairIO.h and I3DHairIO.cpp                                                    *
*                                                                           *
* Content:                                                                  *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann, Sascha Jüngel                                     *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_I3DHAIRIO_H__
#define __CFORGE_I3DHAIRIO_H__

#include "../../CForge/Core/CForgeObject.h"
#include "T3DHair.hpp"

namespace CForge {

	/**
	* \brief Interface definition for hair import/export plug-ins
	*
	* \todo Do full documentation
	*/

	class I3DHairIO: public CForgeObject {
	public:
		enum Operation: uint8_t {
			OP_LOAD = 0,
			OP_STORE,
		};

		virtual void load(const std::string Filepath, T3DHair<float>* pHair) = 0;
		virtual void store(const std::string Filepath, const T3DHair<float>* pHair) = 0;

		virtual void release(void) = 0;
		virtual bool accepted(const std::string Filepath, Operation Op) = 0;
		virtual std::string pluginName(void)const;

	protected:
		I3DHairIO(const std::string ClassName);
		virtual ~I3DHairIO(void);

		void loadCyHairFile(const std::string Filepath, T3DHair<float>* pHair);
		void loadTFXFile(const std::string Filepath, T3DHair<float>* pHair);

	private:
		std::string m_PluginName;
	};//I3DHairIO

}//name space


#endif