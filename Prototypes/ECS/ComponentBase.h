/*****************************************************************************\
*                                                                           *
* File(s): ComponentBase.h and ComponentBase.cpp                              *
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
#ifndef __CFORGE_COMPONENTBASE_H__
#define __CFORGE_COMPONENTBASE_H__

#include <crossforge/Core/CoreDefinitions.h>

namespace CForge {
	class ComponentBase {
	public:
		inline static const std::string identification = "ComponentBase";

		ComponentBase(const std::string identification);
		~ComponentBase();

		const std::string GetIdentification();


	protected:
		std::string m_identification;
	};

	typedef std::shared_ptr<ComponentBase> ComponentBasePtr;
}

#endif