/*****************************************************************************\
*                                                                           *
* File(s): AssociatedWindowComponent.h and AssociatedWindowComponent.cpp    *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_ASSOCIATEDWINDOWCOMPONENT_H__
#define __CROSSFORGE_ASSOCIATEDWINDOWCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class AssociatedWindowComponent : public ComponentBase {
	public:
		inline static std::string identification = "AssociatedWindowComponent";

		AssociatedWindowComponent();
		~AssociatedWindowComponent();

		int64_t &windowEntityId();

	protected:

		int64_t m_windowEntityId;
	};

	typedef std::shared_ptr<AssociatedWindowComponent> AssociatedWindowComponentPtr;
}

#endif 