/*****************************************************************************\
*                                                                           *
* File(s): WindowSystem.h and WindowSystem.cpp                              *
*                                                                           *
* Content:                            *
*                                                                           *
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
#ifndef __CFORGE_WINDOWSYSTEM_H__
#define __COFRGE_WINDOWSYSTEM_H__

#include <crossforge/Core/SLogger.h>
#include "../../ECS/SystemBase.h"
#include "../Entities/WindowEntity.h"

namespace CForge {
	class WindowSystem : public SystemBase {
	public:
		inline static std::string identification = "WindowSystem";

		WindowSystem();
		~WindowSystem();

		void initialize() override;
		void clear() override;
		void update() override;

		bool isEntityValid(EntityBasePtr pEntity) const override;
		bool initOpenGLWindow(WindowEntityPtr pWinEntity);
		void changeVsync(WindowEntityPtr pEntity, int8_t vsyncFactor);

		void swapBuffers();

	protected:

	};

	typedef std::shared_ptr<WindowSystem> WindowSystemPtr;
}

#endif 
