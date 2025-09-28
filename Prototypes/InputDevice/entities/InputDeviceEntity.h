/*****************************************************************************\
*                                                                           *
* File(s): InputDeviceEntity.h and InputDeviceEntity.cpp                       *
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
#ifndef __CROSSFORGE_INPUTDEVICEENTITY_H__
#define __CROSSFORGE_INPUTDEVICEENTITY_H__

#include <crossforge/ecs/EntityBase.h>
#include "../components/KeyboardDataComponent.h"
#include "../components/MouseDataComponent.h"
#include "../components/AssociatedWindowComponent.h"

namespace crossforge {
	class InputDeviceEntity : public EntityBase {
	public:
		inline static std::string identificaiton = "InputDeviceEntity";

		enum InputDeviceComponents: uint8_t {
			KEYBOARD_DATA_COMPONENT = 0x01,
			MOUSE_DATA_COMPONENT = 0x02,
			ASSOCIATED_WINDOW_COMPONENT = 0x04
		};

		InputDeviceEntity(uint8_t componentBitMask = 0);
		~InputDeviceEntity();

		void initialize(uint8_t componentBitMask);
		void clear();

		MouseDataComponentPtr getMouseDataComponent();
		KeyboardDataComponentPtr getKeyboardDataComponent();
		AssociatedWindowComponentPtr getAssociatedWindowComponent();


	protected:

	};

	typedef std::shared_ptr<InputDeviceEntity> InputDeviceEntityPtr;
}

#endif