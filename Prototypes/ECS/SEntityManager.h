/*****************************************************************************\
*                                                                           *
* File(s): SEntityManagerBase.h and SEntityManagerBase.cpp                    *
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
#ifndef __CFORGE_SENTITYMANAGER_H__
#define __CFORGE_SENTITYMANAGER_H__

#include <crossforge/Core/SLogger.h>
#include <unordered_map>

#include "EntityBase.h"

namespace CForge {

	class SEntityManager {
	public:
		static std::shared_ptr<SEntityManager> Instance();
		static void Clear();


		int64_t RegisterEntity(EntityBasePtr entity);
		bool UnregisterEntity(EntityBasePtr entity);

		template<typename T> std::shared_ptr<T> GetEntity(uint64_t entityId) {
			if (entityId < 1 || entityId >= m_entities.size()) throw IndexOutOfBoundsExcept("entityId");
			std::shared_ptr<T> result = nullptr;
			if (nullptr != m_entities[entityId]) {
				result = std::dynamic_pointer_cast<T>(m_entities[entityId]);
			}
			return result;
		}

		template<typename T> std::vector<std::shared_ptr<T>> GetEntities() {
			std::vector<std::shared_ptr<T>> result;
			for (EntityBasePtr pEntity : m_entities) {
				if (nullptr == pEntity) continue;
				if (0 == pEntity->GetIdentification().compare(T::identification)) {
					result.push_back(pEntity);
				}
			}
			return result;
		}

		~SEntityManager();

	protected:
		static std::shared_ptr<SEntityManager> m_instance;

		SEntityManager();
		

		void Initialize();
		

		//std::unordered_map<std::string, std::list<EntityBasePtr>> m_entityListMap;
		std::vector<EntityBasePtr> m_entities;
	};

	typedef SEntityManager EntityManager;
	typedef std::shared_ptr<SEntityManager> EntityManagerPtr;

}


#endif 