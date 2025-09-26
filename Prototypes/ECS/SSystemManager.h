/*****************************************************************************\
*                                                                           *
* File(s): SystemManager.h and SystemManager.cpp                            *
*                                                                           *
* Content:                            *
*                                                                           *
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
#ifndef __CFORGE_SSYSTEMMANAGER_H__
#define __CFORGE_SSYSTEMMANAGER_H__

#include <crossforge/Core/SLogger.h>
#include "SystemBase.h"

namespace CForge {
	class SSystemManager {
	public:
		static std::shared_ptr<SSystemManager> getInstance();
		void clear();

		bool hasSystem(const std::string identification);
		bool addSystem(SystemBasePtr pSystem);
		bool removeSystem(const std::string identification);

		template<typename T>
		std::shared_ptr<T> getSystem(const std::string identification) {
			auto sys = m_systemsMap.find(identification);
			return (m_systemsMap.end() == sys) ? nullptr : std::static_pointer_cast<T>(sys->second);
		}

		~SSystemManager();

	protected:
		static std::shared_ptr<SSystemManager> m_instance;

		SSystemManager();

		void initialiize();

		std::unordered_map<std::string, SystemBasePtr> m_systemsMap;
	};

	typedef SSystemManager SystemManager;
	typedef std::shared_ptr<SSystemManager> SystemManagerPtr;
}


#endif 