/*****************************************************************************\
*                                                                           *
* File(s): SCrossForgeTestApp.h and SCrossForgeTestApp.cpp                       *
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
#ifndef __CROSSFORGE_SCROSSFORGETESTAPP_H__
#define __CROSSFORGE_SCROSSFORGETESTAPP_H__

#include <crossforge/graphics/systems/WindowSystem.h>
#include <crossforge/core/ApplicationBase.h>
#include <crossforge/graphics/entities/WindowEntity.h>
#include <crossforge/input/systems/KeyboardInputSystem.h>
#include <crossforge/input/systems/MouseInputSystem.h>
#include <crossforge/input/entities/InputDeviceEntity.h>
#include <crossforge/graphics/entities/CanvasEntity.h>

namespace crossforge {
	class SCrossForgeTestApp : public ApplicationBase {
	public:
		static std::shared_ptr<SCrossForgeTestApp> instance();
		static void destory();

		void initialize() override;
		void update() override;


		~SCrossForgeTestApp();
	protected:
		static std::shared_ptr<SCrossForgeTestApp> m_pInstance;

		SCrossForgeTestApp();

		void testFileIO();
		void testImageIO();
		void testTriangleMeshIO();
		void testRandom();
		void testShader();
		void testActorCreation();

		void testShaderGeneration();
		void testShaderProvider();

		uint64_t m_startTimestamp;
		uint64_t m_lastPrint;

		WindowEntityPtr m_pMainWin;
		InputDeviceEntityPtr m_pInputDevice;

		std::vector<CanvasEntityPtr> m_canvases;
	};

	typedef SCrossForgeTestApp CrossForgeTestApp;
	typedef std::shared_ptr<SCrossForgeTestApp> CrossForgeTestAppPtr;
}

#endif 