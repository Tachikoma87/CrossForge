/*****************************************************************************\
*                                                                           *
* File(s): STextureManager.h and STextureManager.cpp                             *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_SGUIMANAGER_H__
#define __CFORGE_SGUIMANAGER_H__

#include "../Core/CForgeObject.h"

namespace CForge {
	class CFORGE_API SGUIManager : public CForgeObject {
	public:
		static SGUIManager* instance(void);
		void release(void);
	protected:
		SGUIManager(void);
		~SGUIManager(void);
		void init(void);
	private:
		static uint32_t m_InstanceClount;
		static SGUIManager* m_pInstance;

	};//SGUIManager
}//name space


#endif