/*****************************************************************************\
*                                                                           *
* File(s): WindowEntity.h and WindowEntity.cpp                              *
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
#ifndef __CFORGE_WINDOWENTITY_H__
#define __CFORGE_WINDOWENTITY_H__

#include <crossforge/Core/SLogger.h>
#include "../../ECS/EntityBase.h"
#include "../Components/WindowPropertiesComponent.h"

namespace CForge {
	class WindowEntity : public EntityBase {
	public:
		inline static std::string identification = "WindowEntity";

		WindowEntity();
		~WindowEntity();

		WindowPropertiesComponentPtr getWindowPropertiesComponent();


	protected:

	};

	typedef std::shared_ptr<WindowEntity> WindowEntityPtr;
}

#endif