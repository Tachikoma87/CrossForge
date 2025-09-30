/*****************************************************************************\
*                                                                           *
* File(s): BinaryDataComponent.h and BinaryDataComponent.cpp            *
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
#ifndef __CROSSFORGE_BINARYDATACOMPONENT_H__
#define __CROSSFORGE_BINARYDATACOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class BinaryDataComponent : public ComponentBase {
	public:
		inline static std::string identification = "BinaryDataComponent";

		BinaryDataComponent();
		~BinaryDataComponent();

		void initialize();
		void clear();

		std::vector<uint8_t> &binaryData();
		uint64_t getSize()const;

	protected:
		std::vector<uint8_t> m_binaryData;

	};
	typedef std::shared_ptr<BinaryDataComponent> BinaryDataComponentPtr;
}

#endif 