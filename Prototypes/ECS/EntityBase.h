/*****************************************************************************\
*                                                                           *
* File(s): EntityBase.h and EntityBase.cpp                                  *
*                                                                           *
* Content: Base class for all entities.                           *
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
#ifndef __CFORGE_ENTITYBASE_H__
#define __CFORGE_ENTITYBASE_H__

#include <crossforge/Core/SLogger.h>
#include "ComponentBase.h"
#include <memory>
#include <unordered_map>

namespace CForge {

	class EntityBase {
	public:
		inline static const std::string identification = "EntityBase";

		EntityBase(const std::string identification, int64_t entityId = -1);
		~EntityBase();

		

		bool HasComponent(const std::string identification)const;
		bool AddComponent(ComponentBasePtr component);
		bool RemoveComponent(const std::string identification);
		template<typename T> std::shared_ptr<T> GetComponent(std::string identification) {
			auto result = m_componentMap.find(identification);
			return (m_componentMap.end() == result) ? nullptr : std::static_pointer_cast<T>(result->second);
		}

		int64_t GetEntityId()const;
		void SetEntityId(int64_t entityId);
		const std::string GetIdentification()const;

	protected:
		int64_t m_entityId;
		std::string m_identification;
		std::unordered_map<std::string, ComponentBasePtr> m_componentMap;
	};

	typedef std::shared_ptr<EntityBase> EntityBasePtr;

}

#endif