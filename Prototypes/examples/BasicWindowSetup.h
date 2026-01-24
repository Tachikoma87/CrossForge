/*****************************************************************************\
*                                                                           *
* File(s): BasicWindowSetup.h and BasicWindowSetup.cpp                       *
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
#ifndef __CROSSFORGE_BASICWINDOWSETUP_H__
#define __CROSSFORGE_BASICWINDOWSETUP_H__

#include "ExampleBase.h"

#include <crossforge/input/systems/KeyboardInputSystem.h>
#include <crossforge/input/systems/MouseInputSystem.h>
#include <crossforge/graphics/entities/CanvasEntity.h>


namespace crossforge {
	/**
	* This tutorial example covers creating a OpenGL-capable Window, handling input through mouse and keyboard, computing fps and frame processing times, as well as setting background color.
	*/
	class BasicWindowSetup : public ExampleBase {
	public:

		BasicWindowSetup();
		~BasicWindowSetup();

		void initialize() override;
		void update() override;

	protected:
		
		WindowEntityPtr m_pMainWindow;
		CanvasEntityPtr m_pMainCanvas;

		WindowSystemPtr m_pWindowSystem;

		InputDeviceEntityPtr m_pInputDevice;

		uint64_t m_lastFpsPrint;
		uint64_t m_frameProcessingCounter;
		uint32_t m_frameCounter;

		std::vector<Eigen::Vector4f> m_bgColors;


	};

	using BasicWindowSetupPtr = std::shared_ptr<BasicWindowSetup>;
	using BasicWindowSetupCPtr = std::shared_ptr<const BasicWindowSetup>;
}

#endif 