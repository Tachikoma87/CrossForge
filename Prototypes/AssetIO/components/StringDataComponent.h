/*****************************************************************************\
*                                                                           *
* File(s): StringDataComponent.h and StringDataComponent.cpp            *
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
#ifndef __CROSSFORGE_STRINGDATACOMPONENT_H__
#define __CROSSFORGE_STRINGDATACOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class StringDataComponent : public ComponentBase {
	public:
		inline static std::string identification = "StringDataComponent";

		StringDataComponent();
		~StringDataComponent();

		void initialize();
		void clear();

		std::string& stringData();
		uint64_t length()const;

	protected:
		std::string m_stringData;
	};

	typedef std::shared_ptr<StringDataComponent> StringDataComponentPtr;
}

#endif