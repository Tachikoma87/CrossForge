/*****************************************************************************\
*                                                                           *
* File(s): KeyboardInputSystem.h and KeyboardInputSystem.cpp                *
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
#ifndef __CROSSFORGE_KEYBOARDINPUTSYSTEM_H__
#define __CROSSFORGE_KEYBOARDINPUTSYSTEM_H__

#include <crossforge/ecs/SystemBase.h>
#include "../entities/InputDeviceEntity.h"

namespace crossforge {
	class KeyboardInputSystem : public SystemBase {
	public:
		inline static std::string identification = "KeyboardInputSystem";

		KeyboardInputSystem();
		~KeyboardInputSystem();

		virtual void initialize();
		virtual void clear();
		virtual void update();
		virtual bool isEntityValid(EntityBasePtr pEntity)const;

		virtual bool registerEntity(EntityBasePtr pEntity)override;

	protected:
		/**
		* \brief Keyboard callback method for glfw.
		*
		* \param[in] pWin Associated glfw window, i.e. the window sending the message.
		* \param[in] Key The glfw key.
		* \param[in] Scancode Platform specific key code. Can probably be used to use special keys on certain keyboards.
		* \param[in] Action The action reported for the key (pressed, released, etc.)
		* \param[in] Mods Set of active mods probably. Documentation is not very helpful on that regard.
		*
		*/
		static void keyboardCallbackFunc(struct GLFWwindow* pWin, int Key, int Scancode, int Action, int Mods);

		/**
		* \brief Character callback method for glfw.
		*
		* \param[in] pWin Associated glfw window, i.e. the window sending the message.
		* \param[in] Codepoint Character code.
		*/
		static void characterCallbackFunc(struct GLFWwindow* pWin, unsigned int Codepoint);

		std::unordered_map<void*, std::vector<InputDeviceEntityPtr>> m_handleToEntityMap;
		
	};

	typedef std::shared_ptr<KeyboardInputSystem> KeyboardInputSystemPtr;
}

#endif 