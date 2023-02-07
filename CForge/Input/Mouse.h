/*****************************************************************************\
*                                                                           *
* File(s): Mouse.h and Mouse.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_MOUSE_H__
#define __CFORGE_MOUSE_H__

#include "../Core/CForgeObject.h"

namespace CForge {
	class CFORGE_API Mouse: public CForgeObject {
	public:
		enum Button : int8_t {
			BTN_UNKNOWN = -1,
			BTN_LEFT = 0,
			BTN_RIGHT = 1,
			BTN_MIDDLE = 2,
			BTN_4,
			BTN_5,
			BTN_6,
			BTN_7,
			BTN_8,
			BTN_COUNT,
		};

		Mouse(void);
		~Mouse(void);

		void init(struct GLFWwindow* pWin);
		void clear(void);

		Eigen::Vector2f position(void)const;
		Eigen::Vector2f movement(void)const;
		Eigen::Vector2f wheel(void)const;
		bool buttonState(Button Btn)const;

		void position(Eigen::Vector2f Pos);
		void movement(Eigen::Vector2f Movement);
		void wheel(Eigen::Vector2f Offset);
		void buttonState(Button Btn, bool State);

	protected:
		struct GLFWwindow* m_pWin;
		Eigen::Vector2f m_Position;
		Eigen::Vector2f m_Movement;
		Eigen::Vector2f m_Wheel;
		bool m_BtnState[BTN_COUNT];
	};//Mouse


}//name-space

#endif 