/*****************************************************************************\
*                                                                           *
* File(s): MouseInputSystem.h and MouseInputSystem.cpp                       *
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
#ifndef __CROSSFORGE_MOUSEINPUTSYSTEM_H__
#define __CROSSFORGE_MOUSEINPUTSYSTEM_H__

#include <crossforge/ecs/SystemBase.h>
#include "../entities/InputDeviceEntity.h"

namespace crossforge {
	class MouseInputSystem : public SystemBase {
	public:
		inline static std::string identification = "MouseInputSystem";

		MouseInputSystem();
		~MouseInputSystem();

		virtual bool registerEntity(EntityBasePtr pEntity) override;

		virtual void initialize();
		virtual void clear();
		virtual void update();
		virtual bool isEntityValid(EntityBasePtr pEntity)const;

	protected:

		static std::vector<InputDeviceEntityPtr> getDeviceListFromHandle(void* pHandle);

		/**
		* \brief Mouse position callback method for glfw.
		*
		* \param[in] pWin Associated glfw window, i.e. window sending the message.
		* \param[in] xPos Position in x direction.
		* \param[in] yPos Position in y direction.
		*/
		static void mousePositionCallbackFunc(struct GLFWwindow* pWin, double xPos, double yPos);

		/**
		* \brief Mouse button callback method for glfw.
		*
		* \param[in] pWin Associated glfw window, i.e. window sending the message.
		* \param[in] Button The button identifier.
		* \param[in] Action The action performed (press/release).
		* \param[in] Mode Purpose of this parameter is unclear.
		*/
		static void mouseButtonCallbackFunc(struct GLFWwindow* pWin, int Button, int Action, int Mode);

		/**
		* \brief Mouse wheel callback method for glfw.
		*
		* \param[in] pWin Associated glfw window, i.e. window sending the message.
		* \param[in] xOffset Offset in x direction (primary direction).
		* \param[in] yOffset Offset in y direction.
		*/
		static void mouseWheelCallbackFunc(struct GLFWwindow* pWin, double xOffset, double yOffset);

		


		std::unordered_map<void*, std::vector<InputDeviceEntityPtr>> m_handelToEntitiesMap;

	};

	typedef std::shared_ptr<MouseInputSystem> MouseInputSystemPtr;
}

#endif 