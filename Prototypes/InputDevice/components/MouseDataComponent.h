/*****************************************************************************\
*                                                                           *
* File(s): MouseDataComponent.h and MouseDataComponent.cpp                       *
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
#ifndef __CROSSFORGE_MOUSEDATACOMPONENT_H__
#define __CROSSFORGE_MOUSEDATACOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class MouseDataComponent : public ComponentBase {
	public:
		inline static std::string identification = "MouseDataComponent";

		/**
		* \brief Available buttons.
		*/
		enum Button : int8_t {
			BUTTON_UNKNOWN = -1,	///< Default value.
			BUTTON_LEFT = 0,		///< Left mouse button.
			BUTTON_RIGHT = 1,		///< Right mouse button.
			BUTTON_MIDDLE = 2,		///< Middle mouse button.
			BUTTON_4,				///< Mouse button 4.
			BUTTON_5,				///< Mouse button 5.
			BUTTON_6,				///< Mouse button 6.
			BUTTON_7,				///< Mouse button 7.
			BUTTON_8,				///< Mouse button 8.
			BUTTON_COUNT,			///< Total number of mouse buttons.
		};

		/**
		* \brief Possible state a key can have.
		*/
		enum ButtonState : int8_t {
			STATE_OFF = -1,
			STATE_RELEASED = 0,	///< Key was released.
			STATE_PRESSED = 1,	///< Key is pressed.
		};

		MouseDataComponent();
		~MouseDataComponent();

		Eigen::Vector2f& position();
		Eigen::Vector2f& positionDelta();
		Eigen::Vector2f& wheelOffset();
		ButtonState& buttonState(const Button btn);


	protected:

		Eigen::Vector2f m_wheelOffset;
		Eigen::Vector2f m_position;
		Eigen::Vector2f m_positionDelta;
		ButtonState m_buttonStates[BUTTON_COUNT];
	};

	typedef std::shared_ptr<MouseDataComponent> MouseDataComponentPtr;
}

#endif 