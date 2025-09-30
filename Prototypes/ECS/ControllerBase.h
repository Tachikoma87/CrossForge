/*****************************************************************************\
*                                                                           *
* File(s): ControllerBase.h and ControllerBase.cpp      *
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
#ifndef __CROSSFORGE_CONTROLLERBASE_H__
#define __CROSSFORGE_CONTROLLERBASE_H__

#include <crossforge/core/CoreDefinitions.h>

namespace crossforge {
	class ControllerBase {
	public:
		inline static std::string identification = "ControllerBase";

		std::string getIdentification()const;
		bool isInstanceOf(const std::string identification);
		std::vector<std::string> getInheritance()const;

		~ControllerBase();
	protected:
		ControllerBase(const std::string identification);

		std::vector<std::string> m_inheritance;

	};
}

#endif 