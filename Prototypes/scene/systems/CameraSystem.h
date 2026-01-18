/*****************************************************************************\
*                                                                           *
* File(s): CameraSystem.h and CameraSystem.cpp                        *
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
#ifndef __CROSSFORGE_CAMERASYSTEM_H__
#define __CROSSFORGE_CAMERASYSTEM_H__

#include <crossforge/eccs/SystemBase.h>
#include <crossforge/input/entities/InputDeviceEntity.h>
#include <crossforge/input/systems/KeyboardInputSystem.h>
#include <crossforge/input/systems/MouseInputSystem.h>
#include <crossforge/eccs/SSystemManager.h>
#include <crossforge/graphics/entities/WindowEntity.h>

namespace crossforge {
	class CameraSystem : public SystemBase {
	public:
		static inline std::string identification = "CameraSystem";

		CameraSystem();
		~CameraSystem();

		void initialize();
		void initialize(WindowEntityPtr pAssociatedWindow);
		void clear();
		void update();
		bool isEntityValid(EntityBasePtr pEntity)const;

	protected:
		CameraSystem(const std::string childIdentification);

		SystemManagerPtr m_pSystemManager;
		InputDeviceEntityPtr m_pInputDevice;
	};

	using CameraSystemPtr = std::shared_ptr<CameraSystem>;
	using CameraSystemCPtr = std::shared_ptr<const CameraSystem>;
}

#endif 