/*****************************************************************************\
*                                                                           *
* File(s): SystemBase.h and SystemBase.cpp                              *
*                                                                           *
* Content: Mesh import/export class using AssImp.                           *
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
#ifndef __CFORGE_SYSTEMBASE_H__
#define __CFORGE_SYSTEMBASE_H__

#include <crossforge/Core/CoreDefinitions.h>
#include "EntityBase.h"


namespace CForge {
	class SystemBase {
	public:
		inline static const std::string identification = "SystemBase";

		const std::string GetIdentification()const;

		virtual void Initialize() = 0;
		virtual void Clear() = 0;
		virtual void Update() = 0;

		virtual bool IsEntityValid(EntityBasePtr entity)const = 0;
		virtual bool IsEntityRegistered(EntityBasePtr entity)const = 0;

		~SystemBase();

	protected:
		SystemBase(const std::string identification);
		
		std::string m_identification;
	};

	typedef std::shared_ptr<SystemBase> SystemBasePtr;
}

#endif